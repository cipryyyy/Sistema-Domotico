#include "Interface.h"
#include <climits>

//Funzioni helper
int Interface::CPscan(int id) const noexcept {		// Controllare su un device è CP/M
    for (int i = 0; i < CPcounter; i++) {
        if (id == devicesCP[i].getID()) return i;
    }
    return INT_MIN;
}
int Interface::Mscan(int id) const noexcept {
    for (int i = 0; i < Mcounter; i++) {
        if (id == devicesM[i].getID()) return i;
    }
    return INT_MIN;
}
void Interface::updateKW() noexcept{        			//Sommo tutte le variazioni dei Kilowatt dal tempo 0 fino ad ora
    KW = 0;												//Con le programmazioni esce un casino per il calcolo 
    std::vector<double> delta = timeline.getKWs(0, t);
    for (double d : delta) {
        KW += d;				//Aggiorna direttamente la variabile
    }
}

void timeCheck(int t, int start = INT_MIN, int end = INT_MIN) {
    if (t < 0 || t > 1439) throw InvalidTimeException();				//Tempo base
	if (start != INT_MIN) {												//check di inizio routine
		if (start < 0 || start > 1439) throw InvalidTimeException();
		if (start < t) throw NotATimeMachineException();
	}
	if (end != INT_MIN) {											//check di termine routine
		if (end < 0 || end > 1439) throw InvalidTimeException();
		if (end < start) throw NotATimeMachineException();
	}
}
bool NSCcheck(std::string string1, std::string string2) noexcept {
    if (string1.size() != string2.size()) return false;                 //Controllo preliminare
    std::transform(string1.begin(), string1.end(), string1.begin(), ::tolower);
    std::transform(string2.begin(), string2.end(), string2.begin(), ::tolower);

    return string1 == string2;
}
std::string m2h(int m) noexcept {
    //I casi in cui m va fuori dal range 0-1440 sono già coperti dalla timeline
    std::string hour = std::to_string(m/60);
    std::string minute = std::to_string(m % 60);
    return (hour.size() == 1 ? '0' + hour : hour) + ":" + (minute.size() == 1 ? '0' + minute : minute);
}
void Lid(int len) {
    for (int i = 0; i < len; i++) {
        std::cout << "-";
    }
    std::cout << "\n";
}

//Funzioni membro
Interface::Interface(double KW, bool init, int maxDV, int time): maximumKW{KW}, maximumDV{maxDV}, t{time}
{
    KW = 0;     //KW utilizzati, all'inizio sono 0
    timeline.setRange(maximumDV);

    if (init) {
        CPcounter = 6;
        Mcounter = 4;

        devicesCP = {
            CPDevice(&timeline ,  &t , "Lavatrice", 0, 2, 110),
            CPDevice(&timeline, &t, "Lavastoviglie", 1, 1.5, 195),
            CPDevice(&timeline, &t, "Forno microonde", 2, 0.8, 2),
            CPDevice(&timeline, &t, "Asciugatrice", 3, 0.5, 60),
            CPDevice(&timeline, &t, "Tapparelle", 7, 0.3, 1),
            CPDevice(&timeline, &t, "Televisore", 4, 0.2, 60)
        };
        devicesM = {
            ManualDevice(&timeline, &t, "Impianto fotovoltaico", 5, -1.5),     //Pannello al contrario perché aumenta la soglia di KW disponibili
            ManualDevice(&timeline, &t, "Pompa di calore", 6, 2),
            ManualDevice(&timeline, &t, "Scaldabagno", 8, 1),
            ManualDevice(&timeline, &t, "Frigorifero", 9, 0.4)
        };
    }
}

void Interface::turnOn(int id) {
    updateKW();     //Aggiorno il numero di KW usati
	int Cpos = CPscan(id);
	int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {                                    //id 1-5 per i dispositivi a ciclo programmato
        if (KW + devicesCP[Cpos].getConsumo() > maximumKW) {    //Se supero i KW lancio l'eccezione
            throw OverKWException();
        }
        devicesCP[Cpos].turnOn();                       //Altrimenti lo accendo

        //Aggiornamento timeline (con anche lo spegnimento)
        std::cout << devicesCP[Cpos].getNome() + " acceso" << std::endl;
        timeline.addEvent(t, devicesCP[Cpos].getNome() + " acceso", devicesCP[Cpos].getID()+maximumDV, devicesCP[Cpos].getConsumo());
        timeline.addEvent(t+devicesCP[Cpos].getDurataCiclo(), devicesCP[Cpos].getNome() + " spento", devicesCP[Cpos].getID(), -devicesCP[Cpos].getConsumo());
    } else if (Mpos != INT_MIN) {                           //id 6-10 per i dispositivi manuali
        if (KW + devicesM[Mpos].getConsumo() > maximumKW) {     //Se supero i KW lancio l'eccezione
            throw OverKWException();
        }
        devicesM[Mpos].turnOn();                    //Altrimenti lo accendo
        std::cout << devicesM[Mpos].getNome() + " acceso" << std::endl;
        timeline.addEvent(t, devicesM[Mpos].getNome() + " acceso", devicesM[Mpos].getID()+maximumDV, devicesM[Mpos].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}
void Interface::turnOn(int id, int start) {
    timeCheck(t, start);			//Controllo e aggiorno
	updateKW();

    //Ci possono essere più richieste di timer, quindi controllo se, in base alle richieste future
    //avrò abbastanza KW disponibili per far andare il dispositivo
    double KWonCall = KW;
    std::vector<double> programmedKW = timeline.getKWs(t, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestesKW : programmedKW) {
        KWonCall += requestesKW;                        //Sommo il tutto, dopo devo controllare di averne abbastanza liberi
    }

	int Cpos = CPscan(id);
	int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {   //CP
		//Controllo che il device non sia già attivo al lancio della routine
        std::vector<int> idRequest = timeline.getIDs(0,start);
        for (int i = idRequest.size() - 1; i > 0; i--) {
            if (idRequest[i] == Cpos + maximumDV) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == Cpos) {
                break;
            }
        }

		//Controllo che il device non presenti routine più brevi nella finestra temporale del ciclo
        idRequest = timeline.getIDs(start,start+devicesCP[Cpos].getDurataCiclo());
        for (int i = idRequest.size() - 1; i > 0; i--) {
            if (idRequest[i] == Cpos + maximumDV) {
                throw TimerAlreadySetException();
            }
        }

        // Il dispositivo non sarà attivo nella finestra di tempo, ora controllo che non passi i KW
        if (KWonCall + devicesCP[Cpos].getConsumo() > maximumKW) throw OverKWException();

        //Ora ho tutti i requisiti per impostare la programmazione
        std::cout << "Routine di " << devicesCP[Cpos].getNome() + " impostata" << std::endl;
        timeline.addEvent(start, devicesCP[Cpos].getNome() + " acceso", devicesCP[Cpos].getID()+maximumDV, devicesCP[Cpos].getConsumo());
        timeline.addEvent(start+devicesCP[Cpos].getDurataCiclo(), devicesCP[Cpos].getNome() + " spento", devicesCP[Cpos].getID(), -devicesCP[Cpos].getConsumo());
    } else if (Mpos != INT_MIN) {
        std::vector<int> idRequest = timeline.getIDs(0,start);
        for (int i = idRequest.size() - 1; i > 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (idRequest[i] == Mpos + maximumDV) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == Mpos) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        //Controllo di avere abbastanza KW disponibili
        if (KWonCall + devicesM[Mpos].getConsumo() > maximumKW) throw OverKWException();
        //Se nel mentre erano già presenti delle programmazioni non mi interessa, le cancello tutte direttamente
        timeline.forget(Mpos, start);
        std::cout << "Routine di " << devicesM[Mpos].getNome() + " impostata" << std::endl;
        timeline.addEvent(t, devicesM[Mpos].getNome() + " acceso", devicesM[Mpos].getID()+maximumDV, devicesM[Mpos].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();
    }
}
void Interface::turnOn(int id, int start, int end) {
    timeCheck(t, start, end);
    updateKW();     //Aggiorno il numero di KW usati
    double temp = KW;
    std::vector<double> programmedKW = timeline.getKWs(t, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestedKW : programmedKW) {
        temp += requestedKW;                        //Sommo il tutto         
    }

	int Cpos = CPscan(id);
	int Mpos = Mscan(id);
    if (Cpos != INT_MIN) {
        throw CPIllegalInstructionException();
    } else if (Mpos != INT_MIN) {
        std::vector<int> idRequest = timeline.getIDs(0, start);
        for (int i = idRequest.size() - 1; i > 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (idRequest[i] == Mpos + maximumDV) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == Mpos) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        idRequest = timeline.getIDs(start, end);  //Controllo che non inizino altri timer senza concludersi
        int count = 0;
        for (int i : idRequest) {
            if (i == Mpos + maximumDV) count++;            //Conto che il numero di accensioni sia uguale a quello di spegnimenti
            if (i == Mpos) count--;
        }
        if (count != 0) throw TimerAlreadySetException();
        timeline.forget(Mpos, start, end);        //Cancello queste programmazioni più corte
        //Controllo di avere abbastanza KW al momento del lancio, in tal caso, imposto il programma
        if (temp + devicesM[Mpos].getConsumo() > maximumKW) throw OverKWException();
        std::cout << "Routine di " << devicesM[Mpos].getNome() + " impostata" << std::endl;
        timeline.addEvent(start, devicesM[Mpos].getNome() + " acceso", devicesM[Mpos].getID()+maximumDV, devicesM[Mpos].getConsumo());
        timeline.addEvent(end, devicesM[Mpos].getNome() + " spento", devicesM[Mpos].getID()+maximumDV, -devicesM[Mpos].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::turnOff(int id) {
    updateKW();     //Aggiorno il numero di KW usati

	int Cpos = CPscan(id);
	int Mpos = Mscan(id);
    if (Cpos != INT_MIN) {
        throw CPIllegalInstructionException();  //Se il dispositivo è a ciclo prefissato non si può spegnere manualmente
    } else if (Mpos != INT_MIN) {
        timeline.forget(Mpos, t);     //Cancello eventuali programmazioni future
        std::cout << devicesM[Mpos].getNome() + " spento" << std::endl;
        timeline.addEvent(t, devicesM[Mpos].getNome() + " spento", devicesM[Mpos].getID()+maximumDV, -devicesM[Mpos].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}

void Interface::removeTimer(int id) {
	//Controllo che l'ID esista
	if ((CPscan(id) == INT_MIN && Mscan(id) == INT_MIN) == false) throw DeviceIDOutOfBoundException();
    timeline.forget(id, t);             //Elimino tutti gli eventi futuri legati all'elettrodomestico
}

void Interface::setTime(int time) {
    timeCheck(t, time);                                                 // Controllo che il tempo sia valido

    std::vector<int> timestamp = timeline.getTimes(t,time);             // Vector con gli orari
    std::vector<std::string> events = timeline.getEvents(t,time);       // Vector con gli eventi

    // Stampo la timeline
    std::cout << "[" << m2h(t) << "]: L'orario attuale e' " << m2h(t) << std::endl;
    
    for (size_t i = 0; i < timestamp.size(); ++i) {
        std::cout << "[" << m2h(timestamp[i]) << "]: " << events[i] << std::endl;
    }
    
    std::cout << "[" << m2h(time) << "]: L'orario attuale e' " << m2h(time) << std::endl;
    
    t = time; // Aggiorno l'orario
}

void Interface::resetTime() {
    t = 0;                  //Ritorno con il tempo a 0
    timeline.clear();       //Elimino tutto dalla timeline
    std::cout << "Reset del tempo effettuato con successo" << std::endl;
}

void Interface::show() {
    std::cout << "[" << m2h(t) << "] Stato dei devices:" << std::endl;

    int cols = 4;
    int nameCol = 21;
    int statusCol = 6;
    int usageCol = 5;
    int runningCol = 4;


    int sum = nameCol + statusCol + usageCol + runningCol + 3 * cols + 1;    //Numero di spazi totali
    Lid(sum);
    //Struttura | COL | COL | COL | COL |
    for (int i = -1; i < Mcounter; i++) {
        if (i < 0) {
            int diff;
            std::string headers[5] = {"Nome Device", "Stato ", "KW", "Ore "};
            std::string emptyspaces1;
            std::string emptyspaces2;

            diff = nameCol - headers[0].size();
            for (int i = 0; i < diff; i++) {
                emptyspaces1 += " ";
            }
            diff = usageCol - headers[2].size();
            for (int i = 0; i < diff; i++) {
                emptyspaces2 += " ";
            }
            std::cout << "| " << headers[0] << emptyspaces1 << " | " << headers[1] << " | " << headers[2] << emptyspaces2 << " | " << headers[3] << " |" << std::endl;
            Lid(sum);
        } else {        
            int diff;
            std::string emptyspaces1;
            std::string emptyspaces2;

            diff = nameCol - devicesM[i].getNome().size();
            for (int i = 0; i < diff; i++) {
                emptyspaces1 += " ";
            }
            diff = usageCol - std::to_string(devicesM[i].getConsumo()).size();
            for (int i = 0; i < diff; i++) {
                emptyspaces2 += " ";
            }
            std::cout << "| " << devicesM[i].getNome() << emptyspaces1 << " | " << (devicesM[i].isOn()? "acceso" : "spento") << " | " << devicesM[i].getConsumo() << emptyspaces2 << " | " << m2h(devicesM[i].getTempoDiEsecuzione()) << " |" << std::endl;
        }
    }
    for (int i = 0; i < CPcounter; i++) {      
        int diff;
        std::string emptyspaces1;
        std::string emptyspaces2;

        diff = nameCol - devicesCP[i].getNome().size();
        for (int i = 0; i < diff; i++) {
            emptyspaces1 += " ";
        }
        diff = usageCol - std::to_string(devicesCP[i].getConsumo()).size();
        for (int i = 0; i < diff; i++) {
            emptyspaces2 += " ";
        }
        std::cout << "| " << devicesCP[i].getNome() << emptyspaces1 << " | " << (devicesCP[i].isOn()? "acceso" : "spento") << " | " << devicesCP[i].getConsumo() << emptyspaces2 << " | " << m2h(devicesCP[i].getTempoDiEsecuzione()) << " |" << std::endl;
    }
    Lid(sum);
}

void Interface::show(int id) {
    int Cpos = CPscan(id);
    int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        std::cout   << devicesCP[Cpos].getNome()
					<< "[" 
					<< (devicesCP[Cpos].isOn()? "acceso" : "spento") 
					<< "]" 
					<< " ha consumato " 
					<< devicesCP[Cpos].getConsumo() 
					<< "KW, oggi e' stato usato per " 
					<< m2h(devicesCP[Cpos].getTempoDiEsecuzione())
					<< " ore\n";
    } else if (Mpos != INT_MIN) {
        std::cout   << devicesM[Mpos].getNome()
					<< "[" 
					<< (devicesM[Mpos].isOn()? "acceso" : "spento") 
					<< "]" 
					<< " ha consumato " 
					<< devicesM[Mpos].getConsumo() 
					<< "KW, oggi e' stato usato per " 
					<< m2h(devicesM[Mpos].getTempoDiEsecuzione())
					<< " ore \n";
    } else {
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::installM(std::string name, double consumo, bool isOn) {
    //Controllo che non sia già presente
    for (int i = 0; i < Mcounter; i++) {
        if (NSCcheck(devicesM[i].getNome(), name)) throw DuplicateDeviceException();
    }
	//Creazione ID
    int id;
    if (Mcounter + CPcounter >= maximumDV) throw DeviceLimitException(); // Eccezione, troppi Device installati
    Mcounter++;
    if (freeID.size() > 0) {			//Se ho degli id liberi, li uso
        id = freeID[0];
        freeID.erase(freeID.begin());
    } else {
        id = Mcounter+CPcounter;		//Altrimenti, ne creo uno nuovo
    }
    devicesM.push_back(ManualDevice(&timeline, &t, name, id, consumo, isOn));

}
void Interface::installCP(std::string name, double consumo, int durataCiclo, bool isOn) {
    //Check che non sia già presente
    for (int i = 0; i < CPcounter; i++) {
        if (NSCcheck(devicesCP[i].getNome(), name)) throw DuplicateDeviceException();
    }
	//Creazione ID
    int id;
    if (Mcounter + CPcounter >= maximumDV) throw DeviceLimitException(); // Eccezione, troppi Device installati
    CPcounter++;
    if (freeID.size() > 0) {		//Se ho degli ID liberi, li uso
        id = freeID[0];
        freeID.erase(freeID.begin());
    } else {
        id = Mcounter+CPcounter;	//Altrimneti, ne creo uno nuovo
    }
    devicesCP.push_back(CPDevice(&timeline, &t, name, id, consumo, durataCiclo, isOn));
}
void Interface::uninstall(int id) {
    for (int i = 0; i < CPcounter; i++) {			//Controllo se il device è CP
        if (devicesCP[i].getID() == id) {
            devicesCP.erase(devicesCP.begin() + id);
            freeID.push_back(id);						//Segno il suo ID come libero
			std::cout << "1";
            return;
        }
    }
    for (int i = 0; i < Mcounter; i++) {			//Controllo tra i device M
        if (devicesM[i].getID() == id) {
            devicesM.erase(devicesM.begin() + id);
            freeID.push_back(id);					//Segno l'ID come libero
			std::cout << "2";
            return;
        }
    }
    throw DeviceIDOutOfBoundException();	//Il device non è presente tra i devices	
}

int Interface::searchID(std::string name) {			//Ritorna l'ID dato il nome
	for (int i = 0; i < CPcounter; i++) {
		if (NSCcheck(devicesCP[i].getNome(), name)) return devicesCP[i].getID();	//Controllo i CP
	}
	for (int i = 0; i < Mcounter; i++) {
		if (NSCcheck(devicesM[i].getNome(), name)) return devicesM[i].getID();	//Controllo gli M
	}
	throw NameNotFoundException();						
}