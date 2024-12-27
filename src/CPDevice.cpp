#include "CPDevice.h"

class orarioAccensioneNonImpostato : public std::exception{};

CPDevice::CPDevice(std::string nome, int ID, double consumo, int durataCiclo, bool stato) : Device(nome, ID, consumo, stato) {
    this->orarioAccensioneAutomatica = -1;
    setDurataCiclo(durataCiclo);
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

