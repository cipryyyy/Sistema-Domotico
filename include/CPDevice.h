//Autore: Buso Riccardo

// LEGGI IL COMMENTO IN DEVICE.H GRZ 

#ifndef CPDEVICE_H
#define CPDEVICE_H

#include "Device.h"

class CPDevice : public Device {
private:
    int durataCiclo;
    int orarioAccensioneAutomatica;

public:
    CPDevice(std::string nome, int ID, double consumo, int durataCiclo, bool stato = false) {
        this->nome = nome;
        this->ID = ID;
        this->consumo = consumo;
        this->stato = stato;    
        this->tempoDiEsecuzione = 0;
        this->durataCiclo = durataCiclo;
        this->orarioAccensioneAutomatica = -1; //non è stato impostato
    }

public:

    int getDurataCiclo();

    void setDurataCiclo(int durata);

    int getOrarioAccensioneAutomatica();

    void setOrarioAccensioneAutomatica(int orario);
};

#endif // CPDEVICE_H