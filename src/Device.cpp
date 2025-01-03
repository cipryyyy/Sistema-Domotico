#include "Device.h"

//costruttore

//no costruttore di default perchè non ha senso creare un device senza nome, ID e consumo

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

int Device::getTempoDiEsecuzione() {
    // Ottieni gli ID e i timestamp dal Timeline usando i puntatori
    std::vector<int> IDs = timeline->getIDs(0, *t);  // Usa '->' per accedere ai metodi di 'timeline', 'dereferenzia' 't'
    std::vector<int> timestamps = timeline->getTimes(0, *t);
    
    int range = timeline->getRange();
    int temp = -1;
    int exec = 0;

    // Scorri tutti gli eventi
    for (int i = 0; i < IDs.size(); i++) {

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
}

double Device::getConsumoTotale() {
    return getTempoDiEsecuzione() * (getConsumo()/60);
}