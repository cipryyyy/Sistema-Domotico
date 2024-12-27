#include "Interface.h"

/* 
TODO list:

* Migliorare BeautyTable e applicarlo alle funzioni show()
* Attendere Device per eventuali migliorie
* UpdateKW forse si può migliorare come calcoli
? File a parte per le funzioni helper
? Integrare isM/isCP con searchM/searchCP
*/ 

//Funzioni helper
bool Interface::isCP(int id) noexcept {		// Controllare su un device è CP/M
    for (int i = 0; i < CPcounter; i++) {
        if (id == devicesCP[i].getID()) return true;
    }
    return false;
}
bool Interface::isM(int id) noexcept {
    for (int i = 0; i < Mcounter; i++) {
        if (id == devicesM[i].getID()) return true;
    }
    return false;
}
int Interface::searchCP(int id) noexcept {		//In caso di isM/CP, ritorna l'indice
    for (int i = 0; i < CPcounter; i++) {
        if (id == devicesCP[i].getID()) return i;
    }
	return -1;	//* Non serve a nulla, solo che dopo il compilatore lancia un warning psceno
}
int Interface::searchM(int id) noexcept {
    for (int i = 0; i < Mcounter; i++) {
        if (id == devicesM[i].getID()) return i;
    }
	return -1;		//Stesso discorso
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
void Interface::updateKW() noexcept{        			//Sommo tutte le variazioni dei Kilowatt dal tempo 0 fino ad ora
    KW = 0;												//Con le programmazioni esce un casino per il calcolo :3
    std::vector<double> delta = timeline.getKWs(0, t);	//? Invece di ricalcolare da 0 a t, fare da old_t a t
    for (double d : delta) {
        KW += d;				//Aggiorna direttamente la variabile
    }
}

// TODO Funzioni grafiche
void Lid(int sum) noexcept{
    for (int i = 0; i < sum; i++) {
        std::cout << "-";           //Ciclo di apertura e chiusura della tabella
    }
}
void Interface::BeautyTable(Device *devicelist) noexcept {
    int cols = 4;
    int nameCol = 21;
    int statusCol = 6;
    int usageCol = 5;
    int runningCol = 4;
    int sum = nameCol + statusCol + usageCol + runningCol + 3 * cols + 1;    //Numero di spazi totali
    //Struttura | COL | COL | COL | COL |
    Lid(sum);
    for (int i = -1; i < 5; i++) {
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
            std::cout << "| " << headers[0] << emptyspaces1 << " | " << headers[1] << " | " << headers[2] << emptyspaces2 << " | " << headers[3] << " |";
            Lid(sum);
        } else {        
            int diff;
            std::string emptyspaces1;
            std::string emptyspaces2;

            diff = nameCol - devicelist[i].getNome().size();
            for (int i = 0; i < diff; i++) {
                emptyspaces1 += " ";
            }
            diff = usageCol - std::to_string(devicelist[i].getConsumo()).size();
            for (int i = 0; i < diff; i++) {
                emptyspaces2 += " ";
            }
            std::cout << "| " << devicelist[i].getNome() << emptyspaces1 << " | " << (devicelist[i].isOn()? "acceso" : "spento") << " | " << devicelist[i].getConsumo() << emptyspaces2 << " | " << m2h(devicelist[i].getTempoDiEsecuzione()) << " |";
        }
    }
    Lid(sum);
}
std::string Interface::m2h(int m) noexcept {
    //I casi in cui m va fuori dal range 0-1440 sono già coperti dalla timeline
    std::string hour = std::to_string(m/60);
    std::string minute = std::to_string(m % 60);
    return (hour.size() == 1 ? '0' + hour : hour) + ":" + (minute.size() == 1 ? '0' + minute : minute);
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
            CPDevice("Lavatrice", 0, 2, 110),
            CPDevice("Lavastoviglie", 1, 1.5, 195),
            CPDevice("Forno microonde", 2, 0.8, 2),
            CPDevice("Asciugatrice", 3, 0.5, 60),
            CPDevice("Tapparelle", 7, 0.3, 1),
            CPDevice("Televisore", 4, 0.2, 60)
        };
        devicesM = {
            ManualDevice("Impianto fotovoltaico", 5, -1.5),     //Pannello al contrario perché aumenta la soglia di KW disponibili
            ManualDevice("Pompa di calore", 6, 2),
            ManualDevice("Scaldabagno", 8, 1),
            ManualDevice("Frigorifero", 9, 0.4)
        };
    }
}
void Interface::turnOn(int id) {
    updateKW();     //Aggiorno il numero di KW usati
    if (isCP(id)) {                                    //id 1-5 per i dispositivi a ciclo programmato
        id = searchCP(id);
        if (KW + devicesCP[id].getConsumo() > maximumKW) {    //Se supero i KW lancio l'eccezione
            throw OverKWException();
        }
        devicesCP[id].turnOn();                       //Altrimenti lo accendo

        //Aggiornamento timeline (con anche lo spegnimento)
        timeline.addEvent(t, devicesCP[id].getNome() + " acceso", devicesCP[id].getID()+maximumDV, devicesCP[id].getConsumo());
        timeline.addEvent(t+devicesCP[id].getDurataCiclo(), devicesCP[id].getNome() + " spento", devicesCP[id].getID(), -devicesCP[id].getConsumo());
    } else if (isM(id)) {                           //id 6-10 per i dispositivi manuali
        id = searchM(id);
        if (KW + devicesM[id].getConsumo() > maximumKW) {     //Se supero i KW lancio l'eccezione
            throw OverKWException();
        }
        devicesM[id].turnOn();                    //Altrimenti lo accendo
        timeline.addEvent(t, devicesM[id].getNome() + " acceso", devicesM[id].getID()+maximumDV, devicesM[id].getConsumo());
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

    if (isCP(id)) {   //CP
        id = searchCP(id);
		//Controllo che il device non sia già attivo al lancio della routine
        std::vector<int> idRequest = timeline.getIDs(0,start);
        for (int i = idRequest.size() - 1; i > 0; i--) {
            if (idRequest[i] == id+maximumDV) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == id) {
                break;
            }
        }

		//Controllo che il device non presenti routine più brevi nella finestra temporale del ciclo
        idRequest = timeline.getIDs(start,start+devicesCP[id].getDurataCiclo());
        for (int i = idRequest.size() - 1; i > 0; i--) {
            if (idRequest[i] == id+maximumDV) {
                throw TimerAlreadySetException();
            }
        }

        // Il dispositivo non sarà attivo nella finestra di tempo, ora controllo che non passi i KW
        if (KWonCall + devicesCP[id].getConsumo() > maximumKW) throw OverKWException();

        //Ora ho tutti i requisiti per impostare la programmazione
        timeline.addEvent(start, devicesCP[id].getNome() + " acceso", devicesCP[id].getID()+maximumDV, devicesCP[id].getConsumo());
        timeline.addEvent(start+devicesCP[id].getDurataCiclo(), devicesCP[id].getNome() + " spento", devicesCP[id].getID(), -devicesCP[id].getConsumo());
    } else if (isM(id)) {
        id = searchM(id);
        std::vector<int> idRequest = timeline.getIDs(0,start);
        for (int i = idRequest.size() - 1; i > 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (idRequest[i] == id+maximumDV) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == id) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        //Controllo di avere abbastanza KW disponibili
        if (KWonCall + devicesM[id].getConsumo() > maximumKW) throw OverKWException();
        //Se nel mentre erano già presenti delle programmazioni non mi interessa, le cancello tutte direttamente
        timeline.forget(id, start);
        timeline.addEvent(t, devicesM[id].getNome() + " acceso", devicesM[id].getID()+maximumDV, devicesM[id].getConsumo());
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
    if (isCP(id)) {
        throw CPIllegalInstructionException();
    } else if (isM(id)) {
        id = searchM(id);
        std::vector<int> idRequest = timeline.getIDs(0, start);
        for (int i = idRequest.size() - 1; i > 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (idRequest[i] == id+maximumDV) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == id) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        idRequest = timeline.getIDs(start, end);  //Controllo che non inizino altri timer senza concludersi
        int count = 0;
        for (int i : idRequest) {
            if (i == id+maximumDV) count++;            //Conto che il numero di accensioni sia uguale a quello di spegnimenti
            if (i == id) count--;
        }
        if (count != 0) throw TimerAlreadySetException();
        timeline.forget(id, start, end);        //Cancello queste programmazioni più corte
        //Controllo di avere abbastanza KW al momento del lancio, in tal caso, imposto il programma
        if (temp + devicesM[id].getConsumo() > maximumKW) throw OverKWException();
        timeline.addEvent(start, devicesM[id].getNome() + " acceso", devicesM[id].getID()+maximumDV, devicesM[id].getConsumo());
        timeline.addEvent(end, devicesM[id].getNome() + " spento", devicesM[id].getID()+maximumDV, -devicesM[id].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::turnOff(int id) {
    updateKW();     //Aggiorno il numero di KW usati
    if (isCP(id)) {
        throw CPIllegalInstructionException();  //Se il dispositivo è a ciclo prefissato non si può spegnere manualmente
    } else if (isM(id)) {
        id = searchM(id);
        timeline.forget(id, t);     //Cancello eventuali programmazioni future
        timeline.addEvent(t, devicesM[id].getNome() + " spento", devicesM[id].getID()+maximumDV, -devicesM[id].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}

void Interface::removeTimer(int id) {
	//Controllo che l'ID esista
	if ((isCP(id) || isM(id)) == false) throw DeviceIDOutOfBoundException();
    timeline.forget(id, t);             //Elimino tutti gli eventi futuri legati all'elettrodomestico
}

void Interface::setTime(int time) {
    timeCheck(t, time);

    std::vector<int> timestamp = timeline.getTimes(t,time);             //Vector con gli orari
    std::vector<std::string> events = timeline.getEvents(t,time);       //Vector con gli eventi

    //Stampo tutti gli eventi che sono accaduti in questo lasso di tempo
    std::cout << "[" << m2h(t) << "]: L'orario attuale e' " << m2h(t) << std::endl;
    for (int i = 0; i < timestamp.size(); i++) {
        std::cout << "[" << m2h(timestamp[i]) << "]: " << events[i] << std::endl;
    }
    std::cout << "[" << m2h(time) << "]: L'orario attuale e' " << m2h(time) << std::endl;
    //Aggiorno l'orario
    t = time;
}

void Interface::resetTime() {
    t = 0;                  //Ritorno con il tempo a 0
    timeline.clear();       //Elimino tutto dalla timeline
    std::cout << "Reset del tempo effettuato con successo" << std::endl;
}

void Interface::show() {    //TODO Penso faccia abbastanza schifo per ora
	//stats di tutti i dispositivi
    std::cout << "[" << m2h(t) << "]: Stato dei device manuali:" << std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout << devicesCP[i].getNome()<< "[" << (devicesCP[i].isOn()? "acceso" : "spento") << "]" << " ha consumato " << devicesCP[i].getConsumo() << ", oggi e' stato usato per " << devicesCP[i].getTempoDiEsecuzione() << "\n";
    }
    for (int i = 0; i < 5; i++) {
        std::cout << devicesM[i].getNome()<< "[" << (devicesM[i].isOn()? "acceso" : "spento") << "]" << " ha consumato " << devicesM[i].getConsumo() << ", oggi e' stato usato per " << devicesM[i].getTempoDiEsecuzione() << "\n";
    }
}

void Interface::show(int id) { //TODO Stesso discorso qua
	//stats del device ID
    if (isCP(id)) {
        std::cout << devicesCP[id].getNome()<< "[" << (devicesCP[id].isOn()? "acceso" : "spento") << "]" << " ha consumato " << devicesCP[id].getConsumo() << ", oggi e' stato usato per " << devicesCP[id].getTempoDiEsecuzione() << "\n";
    } else if (isM(id)) {
        std::cout << devicesM[id].getNome()<< "[" << (devicesM[id].isOn()? "acceso" : "spento") << "]" << " ha consumato " << devicesM[id].getConsumo() << ", oggi e' stato usato per " << devicesM[id].getTempoDiEsecuzione() << "\n";
    } else {
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::installM(std::string name, double consumo, bool isOn) {
	//Creazione ID
    int id;
    if (Mcounter + CPcounter >= maximumDV) throw DeviceLimitException();	//Eccezione, troppi Device installati
    Mcounter++;
    if (freeID.size() > 0) {			//Se ho degli id liberi, li uso
        id = freeID[0];
        freeID.erase(freeID.begin());
    } else {
        id = Mcounter+CPcounter;		//Altrimenti, ne creo uno nuovo
    }
    devicesM.push_back(ManualDevice(name, id, consumo, isOn));

}
void Interface::installCP(std::string name, double consumo, int durataCiclo, bool isOn) {
	//Creazione ID
    int id;
    if (Mcounter + CPcounter >= maximumDV) throw DeviceLimitException();	//Eccezione, troppi Device installati
    CPcounter++;
    if (freeID.size() > 0) {		//Se ho degli ID liberi, li uso
        id = freeID[0];
        freeID.erase(freeID.begin());
    } else {
        id = Mcounter+CPcounter;	//Altrimneti, ne creo uno nuovo
    }
    devicesCP.push_back(CPDevice(name, id, consumo, durataCiclo, isOn));
}
void Interface::uninstall(int id) {
    for (int i = 0; i < CPcounter; i++) {			//Controllo se il device è CP
        if (devicesCP[i].getID() == id) {
            devicesCP.erase(devicesCP.begin() + id);
            freeID.push_back(id);						//Segno il suo ID come libero
            return;
        }
    }
    for (int i = 0; i < Mcounter; i++) {			//Controllo tra i device M
        if (devicesM[i].getID() == id) {
            devicesM.erase(devicesM.begin() + id);
            freeID.push_back(id);					//Segno l'ID come libero
            return;
        }
    }
    throw DeviceIDOutOfBoundException();	//Il device non è presente tra i devices	
}

int Interface::searchID(std::string name) {			//Ritorna l'ID dato il nome
	for (int i = 0; i < CPcounter; i++) {
		if (devicesCP[i].getNome() == name) return devicesCP[i].getID();	//Controllo i CP
	}
	for (int i = 0; i < Mcounter; i++) {
		if (devicesM[i].getNome() == name) return devicesM[i].getID();	//Controllo gli M
	}
	throw NameNotFoundException();						
}