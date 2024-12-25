//Autore: Buso Riccardo

//LEGGI IL COMMENTO IN DEVICE.H

#ifndef MANUALDEVICE_H
#define MANUALDEVICE_H

#include "Device.h"
#include <string>

class ManualDevice : public Device {
private:
    int orarioDiSpegnimento; //orario in cui si deve  spegnere, si imposta dall'interfaccia  

public:
    ManualDevice(std::string nome, int ID, double consumo, bool stato = false) {
        this->nome = nome;
        this->ID = ID;
        this->consumo = consumo;
        this->stato = stato;    
        this->tempoDiEsecuzione = 0;
        this->orarioDiSpegnimento = -1; //non è stato impostato
    }

    int getOrarioDiSpegnimento();

    void setOrarioDiSpegnimento(const std::string& shutdownTime);
};

#endif // MANUALDEVICE_H