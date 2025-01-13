//Autore: Buso Riccardo

#ifndef MANUALDEVICE_H
#define MANUALDEVICE_H

#include "Device.h"
#include <string>

class ManualDevice : public Device {
private:
    bool autoTurnOff;   //Il device supporta la politica di spegnimento automatico?
public:
    //Costruttore
    ManualDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool autoTurnOff = true, bool stato = false);
    //Ritorna se supporta la politica
    bool allowAutoTurnOff();
};

#endif // MANUALDEVICE_H