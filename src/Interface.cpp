#include "Interface.h"

/* 
Robe che potrebbero essere funzioni:

* turnOn e turnOff, controlli per i cicli
*/ 

//Funzioni helper
void Lid(int sum) {
    for (int i = 0; i < sum; i++) {
        std::cout << "-";           //Ciclo di apertura e chiusura della tabella
    }
}
void Interface::BeautyTable(Device *devicelist) noexcept {          //!In test
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
void Interface::updateKW() noexcept{        //Sommo tutte le variazioni dei Kilowatt dal tempo 0 fino ad ora
    KW = 0;
    std::vector<double> delta = timeline.getKWs(0, t);
    for (double d : delta) {
        KW += d;
    }
}
std::string Interface::m2h(int m) noexcept {
    //I casi in cui m va fuori dal range 0-1440 sono già coperti dalla timeline
    std::string hour = std::to_string(m/60);
    std::string minute = std::to_string(m % 60);
    return (hour.size() == 1 ? '0' + hour : hour) + ":" + (minute.size() == 1 ? '0' + minute : minute);
}

//Funzioni membro
Interface::Interface(double KW, int time): maximumKW{KW}, t{time}
{
    KW = 0;     //KW utilizzati, all'inizio sono 0
}
void Interface::turnOn(int id) {        //Accensione manuale del device
    updateKW();     //Aggiorno il numero di KW usati
    if (id > 0 && id <= 5) {                                    //id 1-5 per i dispositivi a ciclo programmato
        if (KW + devicesCP[id-1].getConsumo() > maximumKW) {    //Se supero i KW lancio l'eccezione
            throw OverKWException();
        }
        devicesCP[id-1].turnOn();                       //Altrimenti lo accendo
        KW += devicesCP[id-1].getConsumo();

        //Aggiornamento timeline (con anche lo spegnimento)
        timeline.addEvent(t, devicesCP[id-1].getNome() + " acceso", devicesCP[id-1].getID()+10, devicesCP[id-1].getConsumo());
        timeline.addEvent(t+devicesCP[id-1].getDurataCiclo(), devicesCP[id-1].getNome() + " spento", devicesCP[id-1].getID(), -devicesCP[id-1].getConsumo());
    } else if (id >= 6 && id <= 10) {                           //id 6-10 per i dispositivi manuali
        if (KW + devicesM[id-1].getConsumo() > maximumKW) {     //Se supero i KW lancio l'eccezione
            throw OverKWException();
        }
        devicesM[id-6].turnOn();                    //Altrimenti lo accendo
        KW += devicesM[id-6].getConsumo();
        timeline.addEvent(t, devicesM[id-6].getNome() + " acceso", devicesM[id-6].getID()+10, devicesM[id-6].getConsumo());
    
    } else {
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}
void Interface::turnOn(int id, int start) {         //Accensione programamta
    updateKW();     //Aggiorno il numero di KW usati
    if (start < t) throw NotATimeMachineException();
    if (start < 0 || start > 1439) throw InvalidTimeException();
    //Ci possono essere più richieste di timer, quindi controllo se, in base alle richieste future
    //avrò abbastanza KW disponibili per far andare il dispositivo
    double temp = KW;
    std::vector<double> programmedKW = timeline.getKWs(t, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestesKW : programmedKW) {
        temp += requestesKW;                        //Sommo il tutto, dopo devo controllare di averne abbastanza liberi
    }
    if (id >= 1 && id <= 5) {   //CP
        std::vector<int> idRequest = timeline.getIDs(0,start);
        for (int i = idRequest.size() - 1; i > 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (idRequest[i] == id+10) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == id) {
                break;
            }
        }
        idRequest = timeline.getIDs(start,start+devicesCP[id-1].getDurataCiclo());
        for (int i = idRequest.size() - 1; i > 0; i--) {    //Controllo che il device non sia già attivo nella finestra temporale
            if (idRequest[i] == id+10) {
                throw TimerAlreadySetException();
            }
        }
        // Il dispositivo non sarà attivo nella finestra di tempo, ora controllo che non passi i KW
        if (temp + devicesCP[id-1].getConsumo() > maximumKW) throw OverKWException();

        //Ora ho tutti i requisiti per impostare la programmazione
        timeline.addEvent(start, devicesCP[id-1].getNome() + " acceso", devicesCP[id-1].getID()+10, devicesCP[id-1].getConsumo());
        timeline.addEvent(start+devicesCP[id-1].getDurataCiclo(), devicesCP[id-1].getNome() + " spento", devicesCP[id-1].getID(), -devicesCP[id-1].getConsumo());
    } else if (id >= 6 && id <= 10) {
        std::vector<int> idRequest = timeline.getIDs(0,start);
        for (int i = idRequest.size() - 1; i > 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (idRequest[i] == id+10) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == id) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        //Controllo di avere abbastanza KW disponibili
        if (temp + devicesM[id-6].getConsumo() > maximumKW) throw OverKWException();
        //Se nel mentre erano già presenti delle programmazioni non mi interessa, le cancello tutte direttamente
        timeline.forget(id, start);
        timeline.addEvent(t, devicesM[id-6].getNome() + " acceso", devicesM[id-6].getID()+10, devicesM[id-6].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();
    }
}
void Interface::turnOn(int id, int start, int end) {        //Accensione programmata con spegnimento, solo M
    updateKW();     //Aggiorno il numero di KW usati
    if (start < t || end >= start) throw NotATimeMachineException();
    if ((start < 0 || start > 1439) || (end < 0 || end > 1439)) throw InvalidTimeException();
    double temp = KW;
    std::vector<double> programmedKW = timeline.getKWs(t, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestedKW : programmedKW) {
        temp += requestedKW;                        //Sommo il tutto         
    }
    if (id >= 1 && id <= 5) {
        throw CPIllegalInstructionException();
    } else if (id >= 6 && id <= 10) {
        std::vector<int> idRequest = timeline.getIDs(0, start);
        for (int i = idRequest.size() - 1; i > 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (idRequest[i] == id+10) {
                throw TimerAlreadySetException();
            }
            if (idRequest[i] == id) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        idRequest = timeline.getIDs(start, end);  //Controllo che non inizino altri timer senza concludersi
        int count = 0;
        for (int i : idRequest) {
            if (i == id+10) count++;            //Conto che il numero di accensioni sia uguale a quello di spegnimenti
            if (i == id) count--;
        }
        if (count != 0) throw TimerAlreadySetException();
        timeline.forget(id, start, end);        //Cancello queste programmazioni più corte
        //Controllo di avere abbastanza KW al momento del lancio, in tal caso, imposto il programma
        if (temp + devicesM[id-6].getConsumo() > maximumKW) throw OverKWException();
        timeline.addEvent(start, devicesM[id-6].getNome() + " acceso", devicesM[id-6].getID()+10, devicesM[id-6].getConsumo());
        timeline.addEvent(end, devicesM[id-6].getNome() + " spento", devicesM[id-6].getID()+10, -devicesM[id-6].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::turnOff(int id) {       //Spegnimento manuale dei dispositivi, solo M
    updateKW();     //Aggiorno il numero di KW usati
    if (id > 0 && id <= 5) {
        throw CPIllegalInstructionException();  //Se il dispositivo è a ciclo prefissato non si può spegnere manualmente
    } else if (id >= 6 && id <= 10) {
        timeline.forget(id, t);     //Cancello eventuali programmazioni future
        timeline.addEvent(t, devicesM[id-6].getNome() + " spento", devicesM[id-6].getID()+10, -devicesM[id-6].getConsumo());
    } else {
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}

void Interface::removeTimer(int id) {
    timeline.forget(id, t);             //Elimino tutti gli eventi futuri legati all'elettrodomestico
}

void Interface::setTime(int time) {
    if (time < t) throw NotATimeMachineException();                 //Controllo che i dati siano corretti
    if (time < 0 || time > 1439) throw InvalidTimeException();

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
    //? Spegno tutti i dispositivi? Boh, da valutare
}

//TODO Penso faccia abbastanza schifo per ora
void Interface::show() {
    std::cout << "[" << m2h(t) << "]: Stato dei device manuali:" << std::endl;
    for (int i = 0; i < 5; i++) {
        std::cout << devicesCP[i].getNome()<< "[" << (devicesCP[i].isOn()? "acceso" : "spento") << "]" << " ha consumato " << devicesCP[i].getConsumo() << ", oggi e' stato usato per " << devicesCP[i].getTempoDiEsecuzione();
    }
    for (int i = 0; i < 5; i++) {
        std::cout << devicesM[i].getNome()<< "[" << (devicesM[i].isOn()? "acceso" : "spento") << "]" << " ha consumato " << devicesM[i].getConsumo() << ", oggi e' stato usato per " << devicesM[i].getTempoDiEsecuzione();
    }
}
//TODO Stesso discorso qua
void Interface::show(int id) {
    if (id >= 1 && id <= 5) {
        std::cout << devicesCP[id].getNome()<< "[" << (devicesCP[id].isOn()? "acceso" : "spento") << "]" << " ha consumato " << devicesCP[id].getConsumo() << ", oggi e' stato usato per " << devicesCP[id].getTempoDiEsecuzione();
    } else if (id >= 6 && id <= 10) {
        std::cout << devicesM[id].getNome()<< "[" << (devicesM[id].isOn()? "acceso" : "spento") << "]" << " ha consumato " << devicesM[id].getConsumo() << ", oggi e' stato usato per " << devicesM[id].getTempoDiEsecuzione();
    } else {
        throw DeviceIDOutOfBoundException();
    }
}