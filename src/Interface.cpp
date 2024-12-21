#include "Interface.h"

/*Azioni ripetute

turnOn e turnOff, controlli per i cicli
convertitore minuti->24hr
*/ 

//Funzioni helper
void Interface::updateKW() noexcept{        //Sommo tutte le variazioni dei Kilowatt dal tempo 0 fino ad ora
    KW = 0;
    std::vector<double> delta = timeline.getKWs(0, t);
    for (double d : delta) {
        KW += d;
    }
}
bool Interface::Check() {
    return true;                //TODO
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
    if (start < t) {
        throw NotATimeMachineException();
    }
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
    if (start < t || end >= start) {
        throw NotATimeMachineException();
    }
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
    if (time < t) {
        throw NotATimeMachineException();
    }
    /* 
    Modificare il valore di t
    verificare nel lasso di tempo old_t - new_t cos'è successo con le funzioni di timeline
    */
    return;
}

void Interface::resetTime() {
    t = 0;
    timeline.clear();
}

//TODO alla fine
void Interface::show() {
}
//TODO alla fine
void Interface::show(int id) {
}