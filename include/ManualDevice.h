//Autore: Buso Riccardo

#ifndef MANUALDEVICE_H
#define MANUALDEVICE_H

#include "Device.h"
#include <string>

class ManualDevice : public Device {
private:
    bool autoTurnOff;
public:
<<<<<<< HEAD
    ManualDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool autoTurnOff = true, bool stato = false);
    bool allowAutoTurnOff();
=======
    ManualDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato = false);

    int getOrarioDiSpegnimento();

    void setOrarioDiSpegnimento(int shutdownTime);
>>>>>>> main
};

#endif // MANUALDEVICE_H