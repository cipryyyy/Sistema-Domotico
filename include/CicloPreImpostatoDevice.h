#ifndef CICLOPREIMPOSTATODEVICE_H
#define CICLOPREIMPOSTATODEVICE_H

#include "Device.h"

class CicloPreImpostatoDevice : public Device {
private:
    int durataCiclo;
    int orarioAccensioneAutomatica;

public:
     CicloPreImpostatoDevice(std::string nome, int ID, int consumo, bool stato, int durataCiclo) {
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

#endif // CICLOPREIMPOSTATODEVICE_H