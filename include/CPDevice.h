//Autore: Buso Riccardo

// LEGGI IL COMMENTO IN DEVICE.H GRZ 

#ifndef CPDEVICE_H
#define CPDEVICE_H

#include "Device.h"

class CPDevice : public Device {
private:
    int durataCiclo; //in minuti
    int orarioAccensioneAutomatica;

public:
    //costruttore
    CPDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, int durataCiclo, bool stato = false);

public:

    int getDurataCiclo();

    void setDurataCiclo(int durata);

    int getOrarioAccensioneAutomatica();

    void setOrarioAccensioneAutomatica(int orario);
};

#endif // CPDEVICE_H