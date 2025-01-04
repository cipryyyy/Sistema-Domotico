#include "Interface.h"

//Funzioni helper
int Interface::CPscan(int id) const noexcept {		    //Indice e appartenenza di un device alla categoria CP
    for (int i = 0; i < CPcounter; i++) {
        if (id == devicesCP[i].getID()) return i;
    }
    return INT_MIN;    //Se non trovo il device
}
int Interface::Mscan(int id) const noexcept {           //Indice e appartenenza di un device alla categoria M
    for (int i = 0; i < Mcounter; i++) {
        if (id == devicesM[i].getID()) return i;
    }
    return INT_MIN;   //Se non trovo il device
}
void Interface::updateKW() noexcept{        			//Aggiorna il numero di KW in uso
    KW = 0;												//Sommo tutte le variazioni, che con le routine
    std::vector<double> delta = timeline.getKWs(0, t);  //nascono problemi
    for (double d : delta) {
        KW += d;
    }
}

void timeCheck(int t, int start = INT_MIN, int end = INT_MIN) {     //Controllo della validità del tempo
    if (t < 0 || t > 1439) throw InvalidTimeException();				//tempo di input
	if (start != INT_MIN) {												//check di inizio routine
		if (start < 0 || start > 1439) throw InvalidTimeException();
		if (start < t) throw NotATimeMachineException();
	}
	if (end != INT_MIN) {											//check di termine routine
		if (end < 0 || end > 1439) throw InvalidTimeException();
		if (end < start) throw NotATimeMachineException();
	}
}
bool NSCcheck(std::string string1, std::string string2) noexcept {      //Comparazione case unsensitive
    if (string1.size() != string2.size()) return false;                 //Controllo preliminare
    std::transform(string1.begin(), string1.end(), string1.begin(), ::tolower);
    std::transform(string2.begin(), string2.end(), string2.begin(), ::tolower);

    return string1 == string2;  
}
std::string m2h(int m) noexcept {                                   //conversione da minuti a hh:mm
    //I casi in cui m va fuori dal range 0-1440 sono già coperti dalla timeline
    std::string hour = std::to_string(m/60);    
    std::string minute = std::to_string(m % 60);            
    return (hour.size() == 1 ? '0' + hour : hour) + ":" + (minute.size() == 1 ? '0' + minute : minute);
}
void Lid(int len) {                     //hline della tabella
    for (int i = 0; i <= len; i++) {
        std::cout << "-";
    }
    std::cout << "\n";
}
void sortTimeBased(std::vector<int>& timestamps, std::vector<std::string>& events) {    // Riadattamento dell'insertion sort per ordinare gli eventi in base al tempo
    for (int i = 1; i < timestamps.size(); ++i) {
        int key = timestamps[i];
        std::string eventKey = events[i];
        int j = i - 1;

        while (j >= 0 && timestamps[j] > key) {
            timestamps[j + 1] = timestamps[j];
            events[j + 1] = events[j];
            --j;
        }

        timestamps[j + 1] = key;
        events[j + 1] = eventKey;
    }
}
std::string _cleaner(std::string raw) {     //Rimuove gli underscore dai nomi in output
    std::string output = "";
    for (char c : raw) {
        if (c == '_') output+=" ";
        else output += c;
    }
    return output;
}

//Funzioni membro
Interface::Interface(double KW, bool init, int maxDV, int time): maximumKW{KW}, maximumDV{maxDV}, t{time}   //Costruttore
{
    KW = 0;                         //KW utilizzati, all'inizio sono 0
    timeline.setRange(maximumDV);

    if (init) {             //Inizializzazione dei dispositivi secondo la consegna
        CPcounter = 6;
        Mcounter = 4;

        devicesCP = {
            CPDevice(&timeline, &t, "Lavatrice", 0, 2, 110),
            CPDevice(&timeline, &t, "Lavastoviglie", 1, 1.5, 195),
            CPDevice(&timeline, &t, "Forno_microonde", 2, 0.8, 2),
            CPDevice(&timeline, &t, "Asciugatrice", 3, 0.5, 60),
            CPDevice(&timeline, &t, "Tapparelle", 7, 0.3, 1),
            CPDevice(&timeline, &t, "Televisore", 4, 0.2, 60)
        };
        devicesM = {
            ManualDevice(&timeline, &t, "Impianto_fotovoltaico", 5, -1.5),     //Pannello al contrario perché aumenta la soglia di KW disponibili
            ManualDevice(&timeline, &t, "Pompa_di_calore", 6, 2),
            ManualDevice(&timeline, &t, "Scaldabagno", 8, 1),
            ManualDevice(&timeline, &t, "Frigorifero", 9, 0.4, false)
        };
    }
}

void Interface::turnOn(int id) {                        //Accensione manuale
    updateKW();     //Aggiorno il numero di KW usati

	int Cpos = CPscan(id);                        //Controllo se l'ID è tra i dispositivi CP o M
	int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        if (devicesCP[Cpos].isOn())  throw DeviceAlreadyOnException();
        if (KW + devicesCP[Cpos].getConsumo() > maximumKW) {    //Se supero i KW lancio l'eccezione
            throw OverKWException();
        }

        // Se c'è già una routine programmata nella fascia temporale del ciclo, cancello l'accensione
        std::vector<int> IDreq = timeline.getIDs(t, t+devicesCP[Cpos].getDurataCiclo());
        for (int i : IDreq) {
            if (i == id + maximumDV) throw TimerAlreadySetException();
        }

        //Aggiornamento timeline (con anche lo spegnimento)
        std::cout << _cleaner(devicesCP[Cpos].getNome()) + " avviato" << std::endl;
        timeline.addEvent(t, _cleaner(devicesCP[Cpos].getNome()) + " acceso", devicesCP[Cpos].getID()+maximumDV, devicesCP[Cpos].getConsumo());
        timeline.addEvent(t+devicesCP[Cpos].getDurataCiclo(), _cleaner(devicesCP[Cpos].getNome()) + " spento", devicesCP[Cpos].getID(), -devicesCP[Cpos].getConsumo());
    } else if (Mpos != INT_MIN) {
        if (devicesM[Mpos].isOn()) throw DeviceAlreadyOnException();   //Se il dispositivo è già acceso lancio l'eccezione
        if (KW + devicesM[Mpos].getConsumo() > maximumKW) {     //Se supero i KW lancio l'eccezione
            throw OverKWException();
        }
        //Se c'è una routine futura, cancello l'accensione ma mantengo lo spegnimento
        std::vector<int> IDreq = timeline.getIDs(t);
        std::vector<int> TimeRequest = timeline.getTimes(t);
        for (int i = 0; i < IDreq.size(); i++) {
            if (IDreq[i] == id+maximumDV) {
                timeline.forget(id, t, TimeRequest[i]);
            }
        }
        std::cout << _cleaner(devicesM[Mpos].getNome()) + " acceso" << std::endl;
        timeline.addEvent(t, _cleaner(devicesM[Mpos].getNome()) + " acceso", devicesM[Mpos].getID()+maximumDV, devicesM[Mpos].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}
void Interface::turnOn(int id, int start) {             //Accensione programmata
    timeCheck(t, start);			//Controllo e aggiorno
	updateKW();

    //Ci possono essere più richieste di timer, quindi controllo se, in base alle richieste future
    //avrò abbastanza KW disponibili per far andare il dispositivo
    double KWonCall = 0;
    std::vector<double> programmedKW = timeline.getKWs(0, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestesKW : programmedKW) {
        KWonCall += requestesKW;                        //Sommo il tutto, dopo devo controllare di averne abbastanza liberi
    }

	int Cpos = CPscan(id);
	int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        std::vector<int> IDreq = timeline.getIDs(t,start);
        for (int i = IDreq.size() - 1; i >= 0; i--) {   //Controllo che il device non sia già attivo al lancio
            if (IDreq[i] == id + maximumDV) {
                throw TimerAlreadySetException();
            }
            if (IDreq[i] == id) {
                break;
            }
        }

		//Controllo che il device non presenti routine più brevi nella finestra temporale del ciclo
        IDreq = timeline.getIDs(start,start+devicesCP[Cpos].getDurataCiclo());
        for (int i = IDreq.size() - 1; i >= 0; i--) {
            if (IDreq[i] == id + maximumDV) {
                throw TimerAlreadySetException();
            }
        }
        // Il dispositivo non sarà attivo nella finestra di tempo, ora controllo che non passi i KW
        if (KWonCall + devicesCP[Cpos].getConsumo() > maximumKW) throw OverKWException();

        //Ora ho tutti i requisiti per impostare la programmazione
        std::cout << "Accensione di " << _cleaner(devicesCP[Cpos].getNome()) + " impostata" << std::endl;
        timeline.addEvent(start, _cleaner(devicesCP[Cpos].getNome()) + " acceso", devicesCP[Cpos].getID()+maximumDV, devicesCP[Cpos].getConsumo());
        timeline.addEvent(start+devicesCP[Cpos].getDurataCiclo(), _cleaner(devicesCP[Cpos].getNome()) + " spento", devicesCP[Cpos].getID(), -devicesCP[Cpos].getConsumo());
    } else if (Mpos != INT_MIN) {
        std::vector<int> IDreq = timeline.getIDs(0,start);
        for (int i = IDreq.size() - 1; i >= 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (IDreq[i] == id + maximumDV) {
                throw TimerAlreadySetException();
            }
            if (IDreq[i] == id) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        //Controllo di avere abbastanza KW disponibili
        if (KWonCall + devicesM[Mpos].getConsumo() > maximumKW) throw OverKWException();
        //Cancello eventuali programmazioni future
        timeline.forget(Mpos, start);
        std::cout << "Accensione di " << _cleaner(devicesM[Mpos].getNome()) + " impostata" << std::endl;
        timeline.addEvent(t, _cleaner(devicesM[Mpos].getNome()) + " acceso", devicesM[Mpos].getID()+maximumDV, devicesM[Mpos].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();
    }
}
void Interface::turnOn(int id, int start, int end) {    //routine
    timeCheck(t, start, end);
    updateKW();     //Aggiorno il numero di KW usati
    double KWonCall = 0;
    double MaxKWonCall = 0;
    std::vector<double> programmedKW = timeline.getKWs(0, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestedKW : programmedKW) {
        KWonCall += requestedKW;                        //Sommo il tutto         
    }
    programmedKW = timeline.getKWs(start, end); //Controllo che nella routine non si superino mai i maximumKW
    double temp = KWonCall;
    for (double cycleKW : programmedKW) {
        temp += cycleKW;                        //Sommo il tutto  
        if (temp > MaxKWonCall) MaxKWonCall = temp;
    }

	int Cpos = CPscan(id);
	int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        throw CPIllegalInstructionException();  //Se il dispositivo è a ciclo prefissato non si può accendere manualmente
    } else if (Mpos != INT_MIN) {
        std::vector<int> IDreq = timeline.getIDs(0, start);
        for (int i = IDreq.size() - 1; i >= 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (IDreq[i] == id + maximumDV) {
                throw TimerAlreadySetException();
            }
            if (IDreq[i] == id) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        IDreq = timeline.getIDs(start, end);  //Controllo che non inizino altri timer senza concludersi
        int count = 0;
        for (int i : IDreq) {
            if (i == id + maximumDV) count++;            //Conto che il numero di accensioni sia uguale a quello di spegnimenti
            if (i == id) count--;
        }
        if (count != 0) throw TimerAlreadySetException();
        timeline.forget(Mpos, start, end);        //Cancello queste programmazioni più corte
        //Controllo che non si superino i KW
        if (KWonCall + devicesM[Mpos].getConsumo() > maximumKW) throw OverKWException();
        if (MaxKWonCall + devicesM[Mpos].getConsumo() > maximumKW) throw OverKWException();

        //Avvio la routine
        std::cout << "Routine di " << _cleaner(devicesM[Mpos].getNome()) + " impostata" << std::endl;
        timeline.addEvent(start, _cleaner(devicesM[Mpos].getNome()) + " acceso", devicesM[Mpos].getID()+maximumDV, devicesM[Mpos].getConsumo());
        timeline.addEvent(end, _cleaner(devicesM[Mpos].getNome()) + " spento", devicesM[Mpos].getID(), -devicesM[Mpos].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::turnOff(int id) {                       //spegnimento
    updateKW();     //Aggiorno il numero di KW usati

	int Cpos = CPscan(id);
	int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        throw CPIllegalInstructionException();  //Se il dispositivo è a ciclo prefissato non si può spegnere manualmente
    } else if (Mpos != INT_MIN) {
        if (KW - devicesM[Mpos].getConsumo() > maximumKW) throw OverKWException();  //Se cerco di spegnere un generatore e i KW non bastano
        timeline.forget(id, t);     //Cancello eventuali programmazioni future
        std::cout << _cleaner(devicesM[Mpos].getNome()) + " spento" << std::endl;
        timeline.addEvent(t, _cleaner(devicesM[Mpos].getNome()) + " spento", devicesM[Mpos].getID(), -devicesM[Mpos].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}

void Interface::forceOff(int id) noexcept {
    std::vector<int> IDreq = timeline.getIDs(t);
    std::vector<int> TimeStamps = timeline.getTimes(t);

    int Cpos = CPscan(id);
    if (Cpos == INT_MIN) {      //Se non è un dispositivo CP
        turnOff(id);            //Passo il controllo al metodo di spegnimento normale
    }
    for (int i = 0; i < IDreq.size(); i++) {            //Ricerco quando doveva essere spento il device in futuro
        if (IDreq[i] == id) {
            timeline.forget(id, t+1, TimeStamps[i]);        // Aggiorno l'orario di spegnimento
            timeline.addEvent(t, _cleaner(devicesCP[Cpos].getNome()) + " interrotto", devicesCP[Cpos].getID(), -devicesCP[Cpos].getConsumo());
        }
    }
}

void Interface::removeTimer(int id) {                   //Rimuove le routine di un device
	//Controllo che l'ID esista
	if ((CPscan(id) == INT_MIN && Mscan(id) == INT_MIN)) throw DeviceIDOutOfBoundException();
    timeline.forget(id, t);             //Elimino tutti gli eventi futuri legati all'elettrodomestico
}

void Interface::setTime(int time) {                     //Scorrimento del tempo
//? TODO pulire accensioni e spegnimenti contemporanei dei device M
    timeCheck(t, time);                                                 // Controllo che il tempo sia valido

    std::vector<int> timestamp = timeline.getTimes(t,time);             // Vector con gli orari
    std::vector<std::string> events = timeline.getEvents(t,time);       // Vector con gli eventi
    sortTimeBased(timestamp, events);

    // Stampo la timeline
    std::cout << "[" << m2h(t) << "]: L'orario attuale e' " << m2h(t) << std::endl;
    
    for (size_t i = 0; i < timestamp.size(); ++i) {
        std::cout << "[" << m2h(timestamp[i]) << "]: " << events[i] << std::endl;
    }
    
    std::cout << "[" << m2h(time) << "]: L'orario attuale e' " << m2h(time) << std::endl;
    
    t = time; // Aggiorno l'orario
}

void Interface::resetTime() {                           //Resetta il tempo
    t = 0;                  //Ritorno con il tempo a 0
    std::cout << "Reset del tempo effettuato con successo" << std::endl;
}
void Interface::resetTimers() {                         //Resetta tutte le routine
    for (int i = 0; i < CPcounter; i++) {
        timeline.forget(devicesCP[i].getID(), t);
    }
    for (int i = 0; i < Mcounter; i++) {
        timeline.forget(devicesM[i].getID(), t);
    }
}

void Interface::show() {                                //Mostra tutti i device
    std::cout << "[" << m2h(t) << "] Stato dei devices:" << std::endl;
    std::vector<std::string> production;

    int cols = 4;
    int nameCol = 21;
    int statusCol = 6;
    int usageCol = 5;       //Problema
    int runningCol = 4;


    int sum = nameCol + statusCol + usageCol + runningCol + 3 * cols + 1;    //Numero di spazi totali
    Lid(sum);
    //Struttura | COL | COL | COL | COL |
    for (int i = -1; i < Mcounter; i++) {
        if (i < 0) {
            int diff;
            std::string headers[5] = {"Nome Device", "Stato ", "KW", "Ore  "};
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
            if (devicesM[i].getConsumo() < 0) {
                double temp = -devicesM[i].getConsumoTotale();
                production.push_back(_cleaner(devicesM[i].getNome()) + " ha prodotto " + std::to_string(temp) + "KW in " + m2h(devicesM[i].getTempoDiEsecuzione()) + " ore");
                continue;
            }   
            int diff;
            std::string emptyspaces1;

            diff = nameCol - devicesM[i].getNome().size();
            for (int i = 0; i < diff; i++) {
                emptyspaces1 += " ";
            }
            std::cout << "| " << _cleaner(devicesM[i].getNome()) << emptyspaces1 << " | " << (devicesM[i].isOn()? "acceso" : "spento") << " | " << std::fixed << std::setprecision(3) << devicesM[i].getConsumoTotale() << " | " << m2h(devicesM[i].getTempoDiEsecuzione()) << " |" << std::endl;
        }
    }
    for (int i = 0; i < CPcounter; i++) {
        if (devicesCP[i].getConsumo() < 0) {
            double temp = -devicesCP[i].getConsumoTotale();
            production.push_back(_cleaner(devicesCP[i].getNome()) + " ha prodotto " + std::to_string(temp) + "KW in " + m2h(devicesCP[i].getTempoDiEsecuzione()) + " ore");
            continue;
        }
        int diff;
        std::string emptyspaces1;

        diff = nameCol - devicesCP[i].getNome().size();
        for (int i = 0; i < diff; i++) {
            emptyspaces1 += " ";
        }
        std::cout << "| " << _cleaner(devicesCP[i].getNome()) << emptyspaces1 << " | " << (devicesCP[i].isOn()? "acceso" : "spento") << " | " << std::fixed << std::setprecision(3) << devicesCP[i].getConsumoTotale() << " | " << m2h(devicesCP[i].getTempoDiEsecuzione()) << " |" << std::endl;
    }
    Lid(sum);
    for (int i = 0; i < production.size(); i++) {
        std::cout << production[i] << std::endl;
    }
}
void Interface::show(int id) {                          //Mostra un device
    int Cpos = CPscan(id);
    int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        std::cout   << _cleaner(devicesCP[Cpos].getNome())
					<< "[" 
					<< (devicesCP[Cpos].isOn()? "acceso" : "spento") 
					<< "]" 
					<< (devicesCP[Cpos].getConsumo() < 0? " ha prodotto " : " ha consumato") 
					<< (devicesCP[Cpos].getConsumo() < 0? -devicesCP[Cpos].getConsumo() : devicesCP[Cpos].getConsumo())
					<< "KW, oggi e' stato usato per " 
					<< m2h(devicesCP[Cpos].getTempoDiEsecuzione())
					<< " ore\n";
    } else if (Mpos != INT_MIN) {
        std::cout   << _cleaner(devicesM[Mpos].getNome())
					<< "[" 
					<< (devicesM[Mpos].isOn()? "acceso" : "spento") 
					<< "]" 
					<< (devicesM[Mpos].getConsumo() < 0? " ha prodotto " : " ha consumato ")
					<< (devicesM[Mpos].getConsumo() < 0? -devicesM[Mpos].getConsumo() : devicesM[Mpos].getConsumo() )
					<< "KW, oggi e' stato usato per " 
					<< m2h(devicesM[Mpos].getTempoDiEsecuzione())
					<< " ore \n";
    } else {
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::installM(std::string name, double consumo, bool isOn, bool autoTurnOff) {     //Installa device M
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
    devicesM.push_back(ManualDevice(&timeline, &t, name, id, consumo, autoTurnOff, isOn));
    if (consumo < 0) {
        std::cout << "Installato " << _cleaner(name) << "[" << id << "] con produzione " << -consumo << "KW";
    } else {
        std::cout << "Installato " << _cleaner(name) << "[" << id << "] con consumo " << consumo << "KW";
    }
}
void Interface::installCP(std::string name, double consumo, int durataCiclo, bool isOn) {   //Installa device CP
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
        id = Mcounter+CPcounter;	//Altrimenti, ne creo uno nuovo
    }
    devicesCP.push_back(CPDevice(&timeline, &t, name, id, consumo, durataCiclo, isOn));
    if (consumo < 0) {
        std::cout << "Installato " << _cleaner(name) << "[" << id << "] con ciclo " << m2h(durataCiclo) << "h e produzione " << -consumo << "KW";
    } else {
        std::cout << "Installato " << _cleaner(name) << "[" << id << "] con ciclo " << m2h(durataCiclo) << "h e consumo " << consumo << "KW";
    }
}
void Interface::uninstall(int id) {                     //Disinstalla un device
    std::string name;
    for (int i = 0; i < CPcounter; i++) {			//Controllo se il device è CP
        if (devicesCP[i].getID() == id) {
            int pos = CPscan(id);
            if (devicesCP[pos].isOn()) throw DeviceIsRunningException();
            name = devicesCP[i].getNome();
            devicesCP.erase(devicesCP.begin() + pos);
            freeID.push_back(id);						//Segno il suo ID come libero
			CPcounter--;	
            std::cout << "Disintallato il device " << name << "[" << id << "]";
            return;
        }
    }
    for (int i = 0; i < Mcounter; i++) {			//Controllo tra i device M
        if (devicesM[i].getID() == id) {
            int pos = Mscan(id);
            if (devicesM[pos].isOn()) throw DeviceIsRunningException();
            name = devicesM[i].getNome();
            devicesM.erase(devicesM.begin() + pos);
            freeID.push_back(id);					//Segno l'ID come libero
			Mcounter--;	
            std::cout << "Disintallato il device " << name << "[" << id << "]";
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

bool Interface::allowAutoTurnOff(int id) {
    int pos = Mscan(id);
    return devicesM[pos].allowAutoTurnOff();
}

double Interface::debugKWs() {            //Debug KW
    updateKW();
    std::cout << KW;
    return KW;
}
int Interface::debugTime() {              //Debug time
    std::cout << t;
    return t;
}
void Interface::debugCounters() {         //Debug counters
    std::cout << "CP: " << CPcounter << " M:" << Mcounter;
}