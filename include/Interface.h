/*
Autore: Cipriani Andrea
Questa classe importa i device a ciclo programmato e a controllo manuale
Si deve occupare di accendere e spegnere i dispositivi, salvando gli orari di accensione e spegnimento
Inoltre, se viene superato il valore massimo di KW del sistema, il sistema blocca la richiesta (per ora)
Con le funzioni di cambio del tempo vengono mostrati gli eventi impostati
Con le funzioni show si mostrano le statistichen di uno o tutti i dispositivi
*/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include "InterfaceExceptions.h"
#include "CicloPreImpostatoDevice.h"
#include "ManualDevice.h"
#include "Timeline.h"

class Interface {
private:
    int t;
    Timeline timeline;
    Timeline reminders;
    const double maximumKW;
    double KW;

    CicloPreImpostatoDevice devicesCP[5] = {
        CicloPreImpostatoDevice("Lavatrice", 1, 2, 110),
        CicloPreImpostatoDevice("Lavastoviglie", 2, 1.5, 195),
        CicloPreImpostatoDevice("Forno a microonde", 3, 0.8, 2),
        CicloPreImpostatoDevice("Asciugatrice", 4, 0.5, 60),
        CicloPreImpostatoDevice("Televisore", 5, 0.2, 60)
    };
    ManualDevice devicesM[5] = {
        ManualDevice("Impianto fotovoltaico", 6, -1.5),
        ManualDevice("Pompa di calore", 7, 2),
        ManualDevice("Tapparelle", 8, 0.3),
        ManualDevice("Scaldabagno", 9, 1),
        ManualDevice("Frigorifero", 10, 0.4)
    };

public:
    Interface(double KW, int time = 0);

    void turnOn(int id);          //Chiamato con 'set ${DEVICENAME} on'
    void turnOn(int id, int start);          //Chiamato con 'set ${DEVICENAME} on'
    void turnOn(int id, int start, int end);          //Chiamato con 'set ${DEVICENAME} on'
    void turnOff(int id);          //Chiamato con 'set ${DEVICENAME} off'
    void removeTimer(int id);     //Chiamato con rm

    void setTime(int time);     //Passa il tempo in minuti
    void resetTime();   //Imposta t a 0

    void show();      //Mostra la lista di tutti i dispositivi
    void show(int id);    //Mostra lista di dispositivi @riccardoBuso5 buttami tutti i getter, tipo id e cacate varie
};

#endif