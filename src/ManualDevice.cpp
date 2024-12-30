//autore: Buso Riccardo

#include "ManualDevice.h"
#include <string>

    //costr
    ManualDevice::ManualDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato) : Device(timeline, t, nome, ID, consumo, stato) {
        orarioDiSpegnimento = -1;
    }