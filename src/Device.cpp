#include "Device.h"

//costruttore

//no costruttore di default perchè non ha senso creare un device senza nome, ID e consumo

Device::Device(Timeline& timeline, int& t, std::string nome, int ID, double consumo, bool stato){
    this->nome = nome;
    this->ID = ID;
    this->consumo = consumo;
    this->on = stato;    
    this->tempoDiEsecuzione = 0;
    this->timeLine = timeline;
}

Device::~Device() {}

void Device::turnOn() {
    on = true;
    tempoDiEsecuzione = 0;
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

int Device::getTempoDiEsecuzione() const {
    return tempoDiEsecuzione;
}

void Device::setTempoDiEsecuzione(){
    //calcolo il tempo dal momento in cui è stato avviato il mio device, fino a quando è stata chiamata questa funzione

    
}


double Device::getConsumoTot() {
    return tempoDiEsecuzione * consumo;
}

