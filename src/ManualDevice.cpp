//autore: Buso Riccardo

#include "ManualDevice.h"
#include <string>

    //costr
    ManualDevice::ManualDevice(std::string nome, int ID, double consumo, bool stato = false) : Device(nome, ID, consumo, stato) {
        orarioDiSpegnimento = -1;
    }

    int ManualDevice::getOrarioDiSpegnimento() {
        return orarioDiSpegnimento;
    }

    void ManualDevice::setOrarioDiSpegnimento(const std::string& shutdownTime) {
        if(orarioDiSpegnimento < 0 || orarioDiSpegnimento > 1439) {
            throw std::invalid_argument("Orario deve essere compreso tra 0 e 1439 minuti");
        }
        orarioDiSpegnimento = orarioDiSpegnimento;
    }
