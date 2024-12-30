#include "Device.h"

//costruttore

//no costruttore di default perchè non ha senso creare un device senza nome, ID e consumo

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

bool Device::isOn() const {
    return on;
}

std::string Device::getNome() const {
    return nome;
}

void Device::setNome(const std::string& newNome) {
    nome = newNome;
}

int Device::getID() const {
    return ID;
}

void Device::setID(int newID) {
    ID = newID;
}

int Device::getConsumo() const {
    return consumo;
}

void Device::setConsumo(int newConsumo) {
    consumo = newConsumo;
}

void Device::setStato(bool newStato) {
    on = newStato;
}

int Device::getTempoDiEsecuzione() {
    //calcola il tempo di utilizzo del device in minuti durante tutta la giornata

    std::vector<int> ids = timeLine->getIDs(); 
    std::vector<int> times = timeLine->getTimes();
    std::vector<std::string> events = timeLine->getEvents();


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
            if(ids[i] == ID){
                if(events[i] == "on"){
                    //segno il tempo di avvio
                    tempStart = times[i];
                }else{
                    //segno il tempo di spegnimento
                    tempEnd = times[i];
                    //aggiungo il tempo di utilizzo
                    sumTemp += tempEnd - tempStart;
                }
            }
        }
        this->tempoDiEsecuzione = sumTemp;

    }
    return tempoDiEsecuzione;
}

double Device::getConsumoTot() {
    return tempoDiEsecuzione * consumo;
}

