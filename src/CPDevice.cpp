#include "CPDevice.h"

class orarioAccensioneNonImpostato : public std::exception{};

CPDevice::CPDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, int durataCiclo, bool stato) : Device(timeline, t, nome, ID, consumo, stato) {
<<<<<<< HEAD
    if (durataCiclo <= 0) {
        throw ValoreNonValido();
    }
=======
    this->orarioAccensioneAutomatica = -1;
>>>>>>> main
    this->durataCiclo = durataCiclo;
}

int CPDevice::getDurataCiclo() {
    return durataCiclo;
}