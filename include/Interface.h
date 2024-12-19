//Autore: Cipriani Andrea

#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include "InterfaceExceptions.h"
#include "CicloPreImpostatoDevice.h"
#include "ManualDevice.h"

class Interface {
private:
    int t;
    const float maximumKW;
    CicloPreImpostatoDevice devicesCP[5] = {
        CicloPreImpostatoDevice("Lavatrice", 1, 2, 110),
        CicloPreImpostatoDevice("Lavastoviglie", 2, 1.5, 195),
        CicloPreImpostatoDevice("Forno a microonde", 3, 0.8, 2),
        CicloPreImpostatoDevice("Asciugatrice", 4, 0.5, 60),
        CicloPreImpostatoDevice("Televisore", 5, 0.2, 60)
    };
    ManualDevice devicesM[5] = {
        ManualDevice("Impianto fotovoltaico", 6, 1.5),
        ManualDevice("Pompa di calore con termostato", 7, 2),
        ManualDevice("Tapparelle", 8, 0.3),
        ManualDevice("Scaldabagno", 9, 1),
        ManualDevice("Frigorifero", 10, 0.4)
    };

public:
    Interface(float KW, int time = 0);

    void turnOn(int id);          //Chiamato con 'set ${DEVICENAME} on', override per mettere
    void turnOff(int id);          //Chiamato con 'set ${DEVICENAME} off'
    void removeTimer(int id);     //Chiamato con rm

    void setTime(int time);     //Passa il tempo in minuti
    void resetTime();   //Imposta t a 0

    void show(int id);    //Mostra lista di dispositivi @riccardoBuso5 buttami tutti i getter, tipo id e cacate varie
};

#endif