#include "ManualDevice.h"
#include <string>

//costr
ManualDevice::ManualDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool autoTurnOff, bool stato) : Device(timeline, t, nome, ID, consumo, stato) {
    this->autoTurnOff = autoTurnOff;        //Imposto se il Device supporta la politica di spegnimento
}

bool ManualDevice::allowAutoTurnOff() {
    return autoTurnOff;                     //Ritorno il valore
}       