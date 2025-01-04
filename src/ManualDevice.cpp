//autore: Buso Riccardo

#include "ManualDevice.h"
#include <string>

<<<<<<< HEAD
//costr
ManualDevice::ManualDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool autoTurnOff, bool stato) : Device(timeline, t, nome, ID, consumo, stato) {
    this->autoTurnOff = autoTurnOff;
}
=======
    //costr
    ManualDevice::ManualDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato) : Device(timeline, t, nome, ID, consumo, stato) {
        orarioDiSpegnimento = -1;
    }
>>>>>>> main

bool ManualDevice::allowAutoTurnOff() {
    return autoTurnOff;
}