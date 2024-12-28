//Autore: Buso Riccardo

#ifndef MANUALDEVICE_H
#define MANUALDEVICE_H

#include "Device.h"
#include <string>

class ManualDevice : public Device {
private:
    int orarioDiSpegnimento; //orario in cui si deve  spegnere, si imposta dall'interfaccia  

public:
    ManualDevice(Timeline& timeline, int& t, std::string nome, int ID, double consumo, bool stato = false);

    int getOrarioDiSpegnimento();

    void setOrarioDiSpegnimento(const std::string& shutdownTime);
};

#endif // MANUALDEVICE_H