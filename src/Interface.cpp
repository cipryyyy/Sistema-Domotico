#include "Interface.h"

Interface::Interface(double KW, int time): maximumKW{KW}, t{time}
{
    KW = 0;
}
void Interface::turnOn(int id) {        //! Provvisorio, controllare il comportamento CP/M
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
void Interface::turnOn(int id, int start) {
    if (start < t) {
        throw NotATimeMachineException();
    }
    //Ci possono essere più richieste di timer, quindi controllo se, in base alle richieste future
    //avrò abbastanza KW disponibili per far andare il dispositivo

    double temp = KW;
    std::vector<double> programmedKW = timeline.getKWs(t, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestesKW : programmedKW) {
        temp += requestesKW;                        //Sommo il tutto         
    }
    //Se ho abbastanza KW disponibili, programmo l'accensione
    if (id >= 1 && id <= 5) {   //CP
        //! Da riscrivere
        //Richiedo gli id 0-start, se l'ultimo elemento è id+10 lancio l'eccezione, perché sarà ancora attivo
        //Richiedo gli id start-start+duration, se è presente id+10 lancio l'eccezione
        //Altrimenti metto nella timeline l'azione
    } else if (id >= 6 && id <= 10) {
        //! Da riscrivere
        // Richiedo gli id 0-t, se è presente id+10 lancio l'eccezione, perché è già attivo
        // Richiedo gli id t-end, se è presente, forget(id, t) e aggiorno la timeline con la nuova programmazione
    } else {
        throw DeviceIDOutOfBoundException();
    }
}
void Interface::turnOn(int id, int start, int end) {
    if (start < t || end >= start) {
        throw NotATimeMachineException();
    }

    double temp = KW;
    std::vector<double> programmedKW = timeline.getKWs(t, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestedKW : programmedKW) {
        temp += requestedKW;                        //Sommo il tutto         
    }
    //Se ho abbastanza KW disponibili, programmo l'accensione
    if (id >= 1 && id <= 5) {
        //! Da riscrivere
        //I programmi CP non supportano questa programmazione, eccezione
    } else if (id >= 6 && id <= 10) {
        //! Da riscrivere
        // Richiedo gli id 0-start, se l'ultimo stato di id è +10, eccezione
        // Richiedo gli id start-end, se è presente id +10, eccezione
        // Altrimenti, aggiorno la timeline con accensione e spegnimento, dopo end non mi interessa cosa fa il device
    } else {
        throw DeviceIDOutOfBoundException();
    }

}

void Interface::turnOff(int id) {        //TODO
    if (id > 0 && id <= 5) {
        throw CPIllegalInstructionException();  //Se il dispositivo è a ciclo prefissato non si può spegnere manualmente
    } else if (id >= 6 && id <= 10) {
        //Controllo se il dispositivo ha una programmazione
        //Se ce l'ha devo rimuovere la parte in cui comunico lo spegnimento
        //Spengo il dispositivo
    } else {
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}

void Interface::removeTimer(int id) {
    timeline.forget(id, t);             //Elimino tutti gli eventi futuri 
}

void Interface::setTime(int time) {
    if (time < t) {
        throw NotATimeMachineException();
    }
    /* 
    Modificare il valore i t
    verificare nel lasso di tempo old_t - new_t cos'è successo con le funzioni di timeline
    */
    return;
}

void Interface::resetTime() {
    t = 0;
    timeline.clear();
}

void Interface::show() {
    //TODO alla fine
}
void Interface::show(int id) {
    //TODO alla fine
}