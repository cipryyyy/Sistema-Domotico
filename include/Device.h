//Autore: Buso Riccardo

#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include "Timeline.h"


class Device {
protected:
    Timeline* timeline;  // Puntatore a Timeline
    int* t;              // Puntatore a int
    std::string nome;
    int ID;
    double consumo;
    bool on;
    int exec;

public:
    Device(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato = false);

    // Member functions
    virtual bool isOn();

    // Getter and Setter for nome
    std::string getNome() const;

    // Getter and Setter for ID
    int getID() const; 

    // Getter and Setter for consumo
    double getConsumo() const;

    // Getter and Setter for tempoDiEsecuzione
    int getTempoDiEsecuzione();
    void setTempoDiEsecuzione(int time);
    double getConsumoTotale();

    void off();         // imposta on a false, serve per i reset time e reset all
};

#endif // DEVICE_H