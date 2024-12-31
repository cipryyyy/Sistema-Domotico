#include "CPDevice.h"

class orarioAccensioneNonImpostato : public std::exception{};

CPDevice::CPDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, int durataCiclo, bool stato) : Device(timeline, t, nome, ID, consumo, stato) {
    this->orarioAccensioneAutomatica = -1;
    this->durataCiclo = durataCiclo;
    this->tempoDiEsecuzione = -1;
}

int CPDevice::getDurataCiclo() {
    return durataCiclo;
}

void CPDevice::setDurataCiclo(int durata) {
    if(durataCiclo < 0 || durataCiclo > 1439) {
        throw std::invalid_argument("Durata ciclo deve essere compresa tra 0 e 1439 minuti");
    }
    durataCiclo = durata;
}

int CPDevice::getOrarioAccensioneAutomatica() {
    return orarioAccensioneAutomatica;
    //se non impostato ritornerà -1 
}

void CPDevice::setOrarioAccensioneAutomatica(int orario) {
    if(orario < 0 || orario > 1439) {
        throw std::invalid_argument("Orario deve essere compreso tra 0 e 1439 minuti");
    }
    orarioAccensioneAutomatica = orario;
}

//un device a ciclo impostato, può spegnersi solo a ciclo conluso, quindi il suo tempo di esecuzione
// è uguale alla durata del ciclo + tempo di esecuzione, ogni qualvolta venga spento

void CPDevice::turnOff(){
    Device::turnOff();
    this->tempoDiEsecuzione += this->durataCiclo ;
}

