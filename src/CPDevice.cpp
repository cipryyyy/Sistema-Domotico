#include "CPDevice.h"

class orarioAccensioneNonImpostato : public std::exception{};

CPDevice::CPDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, int durataCiclo, bool stato) : Device(timeline, t, nome, ID, consumo, stato) {
    if (durataCiclo <= 0) {         //Il ciclo deve durare almeno un minuto
        throw ValoreNonValido();
    }
    this->durataCiclo = durataCiclo;        //imposto il valore
}

int CPDevice::getDurataCiclo() {
    return durataCiclo;             //Ritorno la durata
}