//Autore: Cipriani Andrea

#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include "CicloPreImpostatoDevice.h"
#include "ManualDevice.h"

class Interface {
private:
    int t;
    const float maximumKW;
    //CicloPreImpostatoDevice *devicesCP;   Mi servono i costruttori di default :)
    //ManualDevice *devicesM;
public:
    Interface(float KW, int time = 0);

    ~Interface();

    void turnOn();          //Chiamato con 'set ${DEVICENAME} on'
    void turnOff();          //Chiamato con 'set ${DEVICENAME} off'
    void removeTimer();

    void setTime();
    void resetTime();

    void show(int id = 0);    //Mostra lista dispositivi, id != 0 per i dispositivi, non ho voglia di fare una funzione a parte
};

#endif