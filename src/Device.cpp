#include "Device.h"

Device::Device(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato) {
        // Uso i puntatori per il tempo e la timeline, così posso accedervi sia da qua che da 'Interface'
        this->timeline = timeline;
        this->t = t;
        //Salvo tutti i metadati
        this->nome = nome;
        this->ID = ID;
        this->consumo = consumo;
        this->on = stato;
    }

bool Device::isOn() {                           //Controllo se è on
    std::vector<int> IDs = timeline->getIDs(0, *t); //Richiedo gli ID
    int range = timeline->getRange();               //Richiedo il range

    for (int i = IDs.size() - 1; i >= 0; i--) {
        if (IDs[i] == ID) {                 //Se l'ultimo evento è di spegnimento
            on = false;                     //Il device è spento
            break;
        }
        if (IDs[i] == ID + range) {         //Se l'ultimo evento è di accensione
            on = true;                      //Il device è acceso
            break;
        }
    }
    return on;                //Ritorno lo stato
}

std::string Device::getNome() const {     //Ritorno il nome
    return nome;
}

int Device::getID() const {             //Ritorno l'ID
    return ID;
}

double Device::getConsumo() const {
    return consumo;
}

int Device::getTempoDiEsecuzione() {                    //Calcolo il tempo di esecuzione
    std::vector<int> IDs = timeline->getIDs(0, *t);  // Usa '->' per accedere ai metodi di 'timeline', 'dereferenzia' 't'
    std::vector<int> timestamps = timeline->getTimes(0, *t);
    
    int range = timeline -> getRange();         //Richiedo il range
    int temp = -1;                              //Temp salva la differenza di tempo tra un accensione e uno spegnimento
    int exec = 0;                               //Exec è il tempo di esecuzione totale

    for (int i = 0; i < IDs.size(); i++) {      //Controllo tutti gli eventi
        if (IDs[i] == ID + range) {             //Se viene acceso, salvo il timestamp
            temp = timestamps[i];
        } else if (IDs[i] == ID) {              //Se viene spento, calcolo la differenza di tempo
            if (temp != -1) {
                exec += timestamps[i] - temp;
                temp = -1;                      //Resetto temp
            }
        }
    }

    if (isOn() && temp != -1) {         //Se il device è acceso, calcolo il tempo di accensione
        exec += *t - temp;              //Dall'ultima accensione al tempo attuale
    }
    return exec;                        //Ritorno il tempo di esecuzione
}

void Device::setTempoDiEsecuzione(int time) {           //Imposta il tempo di esecuzione
    exec = time;
}

double Device::getConsumoTotale() {                     //Consumo nel tempo
    return getTempoDiEsecuzione() * (getConsumo()/60);  //Tempo di esecuzione per il consumo per minuto
}

void Device::off() {                    //Spegni il device
    on = false;
}