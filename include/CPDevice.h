//Autore: Buso Riccardo

// LEGGI IL COMMENTO IN DEVICE.H GRZ 

#ifndef CPDEVICE_H
#define CPDEVICE_H

#include "Device.h"

class CPDevice : public Device {
private:
    int durataCiclo; //in minuti
    int orarioAccensioneAutomatica;
    int tempoDiEsecuzione; //da quanto tempo è accesso, si resetta ogni volta che viene spento

public:
    //costruttore
    CPDevice(Timeline* timeline, int* t, std::string nome, int ID, double consumo, int durataCiclo, bool stato = false);

public:

    int getDurataCiclo();

    void setDurataCiclo(int durata);

    int getOrarioAccensioneAutomatica();

    void setOrarioAccensioneAutomatica(int orario);

    void turnOff() override;
};

#endif // CPDEVICE_H