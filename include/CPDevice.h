//Autore: Buso Riccardo

// LEGGI IL COMMENTO IN DEVICE.H GRZ 

#ifndef CPDEVICE_H
#define CPDEVICE_H

#include "Device.h"
#include <stdexcept>

class CPDevice : public Device {
private:
    int durataCiclo; //in minuti

public:
    class ValoreNonValido : public std::exception {
        const char* what() const noexcept override {
            return "Valore non valido";
        }
    };
    //costruttore
    CPDevice(Timeline *timeline, int *t, std::string nome, int ID, double consumo, int durataCiclo, bool stato = false);
    int getDurataCiclo();
};

#endif // CPDEVICE_H