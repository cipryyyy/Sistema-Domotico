//autore: Buso Riccardo

#include "ManualDevice.h"
#include <string>

    //costr
    ManualDevice::ManualDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato) : Device(timeline, t, nome, ID, consumo, stato) {
        orarioDiSpegnimento = -1;
    }

    int ManualDevice::getOrarioDiSpegnimento() {
        return orarioDiSpegnimento;
    }

    void ManualDevice::setOrarioDiSpegnimento(int shutdownTime) {
        if(shutdownTime < 0 || shutdownTime > 1439) {
            throw std::invalid_argument("Orario deve essere compreso tra 0 e 1439 minuti");
        }
        orarioDiSpegnimento = shutdownTime;
    }
