#include "Device.h"

//costruttore

//no costruttore di default perchè non ha senso creare un device senza nome, ID e consumo

<<<<<<< HEAD
Device::Device(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato) {
        // Usa i puntatori per manipolare gli oggetti passati
        this->timeline = timeline;
        this->t = t;
        this->nome = nome;
        this->ID = ID;
        this->consumo = consumo;
        this->on = stato;
    }

bool Device::isOn() {
    std::vector<int> IDs = timeline->getIDs(0, *t);
    int range = timeline->getRange();
=======
Device::Device(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato){
    this->nome = nome;
    this->ID = ID;
    this->consumo = consumo;
    this->on = stato;    
    this->tempoDiEsecuzione = -1; //il device non è ancora stato usato
    this->timeLine = timeline;
    this-> t = t;
}

Device::~Device() {}

void Device::turnOn() {
    on = true;
}

void Device::turnOff() {
    on = false;
}

bool Device::isOn() {
    std::vector<int> IDs = timeLine->getIDs(0, *t);
    int range = timeLine->getRange();
>>>>>>> main
    
    for (int i = IDs.size() - 1; i >= 0; i--) {
        if (IDs[i] == ID) {
            on = false;
            break;
        }
        if (IDs[i] == ID + range) {
            on = true;
            break;
        }
    }
    return on;
}


std::string Device::getNome() const {
    return nome;
}

int Device::getID() const {
    return ID;
}

double Device::getConsumo() const {
    return consumo;
}

<<<<<<< HEAD
int Device::getTempoDiEsecuzione() {
    // Ottieni gli ID e i timestamp dal Timeline usando i puntatori
    std::vector<int> IDs = timeline->getIDs(0, *t);  // Usa '->' per accedere ai metodi di 'timeline', 'dereferenzia' 't'
    std::vector<int> timestamps = timeline->getTimes(0, *t);
    
    int range = timeline->getRange();
    int temp = -1;
    int exec = 0;
=======
double Device::getConsumoTotale() {
    return 0;
}

void Device::setConsumo(int newConsumo) {
    consumo = newConsumo;
}
>>>>>>> main

    // Scorri tutti gli eventi
    for (int i = 0; i < IDs.size(); i++) {

<<<<<<< HEAD
        if (IDs[i] == ID + range) {
            temp = timestamps[i];
        } else if (IDs[i] == ID) {
            if (temp != -1) {
                exec += timestamps[i] - temp;
                temp = -1;
            }
        }
    }

    if (isOn() && temp != -1) {
        exec += *t - temp;
    }

    return exec;
}

void Device::setTempoDiEsecuzione(int time) {
    exec = time;
=======
int Device::getTempoDiEsecuzione() {
    //calcola il tempo di utilizzo del device in minuti durante tutta la giornata

    std::vector<int> ids = timeLine->getIDs(); 
    std::vector<int> times = timeLine->getTimes();


    bool found = false;

    //cerco se l'ID del device è presente nella timeline
    for(int i=0;i<ids.size();i++){
        if(ids[i] == ID){
            found = true;
            break;
        }
    }
    //se non è presente non è mai stato usato
    if(found == true){
        
        int sumTemp = 0;
        int tempStart = 0;
        int tempEnd = 0;

        for(int i=0; i < times.size(); i++){
            if(ids[i] == ID+1024){
                    //segno il tempo di avvio
                    tempStart = times[i]*60;
                }else{
                    //segno il tempo di spegnimento
                    tempEnd = times[i]*60;
                    //aggiungo il tempo di utilizzo
                    sumTemp += tempEnd - tempStart;
            }
        }
        this->tempoDiEsecuzione = sumTemp;

    }
    return tempoDiEsecuzione;
}

void Device::setTempoDiEsecuzione(int time) {
    tempoDiEsecuzione = time;
}

double Device::getConsumoTot() {
    return (tempoDiEsecuzione * consumo)/60;
>>>>>>> main
}

double Device::getConsumoTotale() {
    return getTempoDiEsecuzione() * (getConsumo()/60);
}