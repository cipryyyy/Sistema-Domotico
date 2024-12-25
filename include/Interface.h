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
#include "InterfaceExceptions.h"
#include "CPDevice.h"
#include "ManualDevice.h"
#include "Timeline.h"

class Interface {
private:
    const int maximumDV;            //Numero massimo di device dell'impianto
    const double maximumKW;         //KW massimi dell'impianto
    double KW;                      //KW attualmente in uso
    int Mcounter;                   //Numero di device M
    int CPcounter;                  //Numero di device CP
    int t;                          //tempo in minuti
    Timeline timeline;              //timeline degli eventi

    bool isCP(int id) noexcept;
    bool isM(int M) noexcept;
    int searchCP(int id) noexcept;
    int searchM(int id) noexcept;
    void updateKW() noexcept;                  //Aggiorna il numero di KW utilizzati ad ogni chiamata
    void BeautyTable(Device *devicelist) noexcept;
    std::string m2h(int minute) noexcept;      //Converte da minuti in formato hh::mm

    std::vector<int> freeID;
    std::vector<CPDevice> devicesCP;
    std::vector<ManualDevice> devicesM;

public:
    Interface(double KW, bool init = true, int maxDV = 1024, int time = 0);

    void turnOn(int id);                                // Chiamato con 'set ${DEVICENAME} on'
    void turnOn(int id, int start);                     // Chiamato con 'set ${DEVICENAME} [start] on'
    void turnOn(int id, int start, int end);            // Chiamato con 'set ${DEVICENAME} [start]-[end]on'
    void turnOff(int id);                               // Chiamato con 'set ${DEVICENAME} off'
    void removeTimer(int id);                           // Chiamato con 'rm ${DEVICENAME}' 

    void setTime(int time);                             //Fa scorrere il tempo, chiamato con 'set time ${TIME}'
    void resetTime();                                   //Imposta t a 0, chiamato con 'reset time'

    void show();                                        //Mostra lo stato di tutti i dispositivi
    void show(int id);                                  //Mostra lo stato di un singolo dispositivo

    void installM(std::string name, double consumo, bool isOn = false);                      //Installa un dispositivo manuale
    void installCP(std::string name, double consumo, int durataCiclo, bool isOn = false);    //Installa un dispositivo CP
    void uninstall(int id);                                                                 //Rimuove un dispositivo

    int searchID(std::string name);                     //Ritorna l'ID di un dispositivo dato il nome       
};  
#endif