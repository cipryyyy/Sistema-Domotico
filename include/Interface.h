/*
Autore: Cipriani Andrea

L'interfaccia si pone tra la parte di parsing (main.cpp) e i gestori del singolo device.
Si occupa di;
Accendere/Spegnere dispositivi
Creare/Rimuovere delle routine di accensione e spegnimento
Far scorrere il tempo
Mostrare i dati e i consumi
Installare devices
Rimuovere devices 

Non si occupa di gestire la politica di spegnimenti in caso di OverKWException, o in generale di gestire errori.
Genera le eccezioni e basta.
*/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include <algorithm>                // Per std::transform
#include <iomanip>                  // Per il setprecision
#include "InterfaceExceptions.h"    // Per le eccezioni
#include "CPDevice.h"               // Per i dispositivi CP
#include "ManualDevice.h"           // Per i dispositivi manuali
#include "Timeline.h"               // Per la timeline

class Interface {
private:
    const int maximumDV;            //Numero massimo di device dell'impianto
    const double maximumKW;         //KW massimi dell'impianto
    double KW;                      //KW attualmente in uso
    int Mcounter;                   //Numero di device M
    int CPcounter;                  //Numero di device CP
    int t;                          //tempo in minuti
    Timeline timeline;              //timeline degli eventi

    int CPscan(int id) const noexcept;          //Ricerca di un device CP
    int Mscan(int M) const noexcept;            //Ricerca di un device M
    void updateKW() noexcept;                  //Aggiorna il numero di KW utilizzati ad ogni chiamata

    std::vector<int> freeID;                    //ID liberi
    std::vector<CPDevice> devicesCP;        //Vector di dispositivi CP
    std::vector<ManualDevice> devicesM;     //Vector di dispositivi M

public:
    Interface(double KW, bool init = true, int maxDV = 1024, int time = 0); //Costruttore

    void turnOn(int id);                                // Chiamato con 'set ${DEVICENAME} on'
    void turnOn(int id, int start);                     // Chiamato con 'set ${DEVICENAME} [start] on'
    void turnOn(int id, int start, int end);            // Chiamato con 'set ${DEVICENAME} [start]-[end]on'
    void turnOff(int id);                               // Chiamato con 'set ${DEVICENAME} off'
    void forceOff(int id) noexcept;                     // Spegnimento forzato dei devices CP
    void removeTimer(int id);                           // Chiamato con 'rm ${DEVICENAME}' 

    void setTime(int time);                             //Fa scorrere il tempo, chiamato con 'set time ${TIME}'
    void resetTime();                                   //Imposta t a 0, chiamato con 'reset time'
    void resetTimers();                                 //Rimuove i timers a partire da t

    void show();                                        //Mostra lo stato di tutti i dispositivi
    void show(int id);                                  //Mostra lo stato di un singolo dispositivo

    void installM(std::string name, double consumo, bool isOn = false, bool autoTurnOff = true); //Installa un dispositivo manuale
    void installCP(std::string name, double consumo, int durataCiclo, bool isOn = false);        //Installa un dispositivo CP
    void uninstall(int id);                                                                      //Rimuove un dispositivo

    //Accesso alle informazioni dei devices
    int searchID(std::string name);                     //Ritorna l'ID di un dispositivo dato il nome
    bool allowAutoTurnOff(int id);                      //Ritorna se il device supporta l'autoTurnOff in caso di OverKWException
    std::vector<int> turnOffSequence();                  //Ritorna il primo dispositivo acceso

    //Funzioni di debug
    double debugKWs();                //Mostra i KW attualmente in uso
    int debugTime();                  //Mostra il tempo attuale
    void debugCounters();             //Mostra il numero di device CP e M
};  
#endif