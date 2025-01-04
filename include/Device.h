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
<<<<<<< HEAD
    Device(Timeline* timeline, int* t, std::string nome, int ID, double consumo, bool stato = false);

    // Member functions
    virtual bool isOn();

=======
    // Constructor

    Device(Timeline* timeline, int* t, std::string nome, int ID, double consumo,bool stato = false);

    // Destructor
    virtual ~Device();

    // Member functions
    virtual void turnOn();
    virtual void turnOff();
    virtual bool isOn();

    // Add more member functions and attributes as needed

protected:
    // attributi 
    std::string nome;    
    int ID; 
    double consumo; //consumo specifico del device
    bool on; //false spento, true accesso
    int tempoDiEsecuzione; //da quanto tempo è accesso, si resetta ogni volta che viene spento 
    Timeline *timeLine ; 
    int *t; //tempo attuale in minuti dall'inizio della giornata



public:             //! Lo ho messo un attimo public che i getter mi servono in Interface.h
>>>>>>> main
    // Getter and Setter for nome
    std::string getNome() const;

    // Getter and Setter for ID
    int getID() const; 

    // Getter and Setter for consumo
    double getConsumo() const;
<<<<<<< HEAD

    // Getter and Setter for tempoDiEsecuzione
    int getTempoDiEsecuzione();
    void setTempoDiEsecuzione(int time);
    double getConsumoTotale();
=======
    double getConsumoTotale() ;
    void setConsumo(int consumo); 
    double setConsumoTotale();

    // Getter and Setter for stato
    bool getStato() const;      //! Ripetuta, c'è già isOn, ma per coerenza con gli altri getter si mette lo stesso
    void setStato(bool stato);

    // Getter and Setter for tempoDiEsecuzione
    int getTempoDiEsecuzione() ;
    void setTempoDiEsecuzione(int time);

     //funzione che calcoli il consumo del device in base al tempo di esecuzione
    virtual double getConsumoTot(); 
>>>>>>> main
};

#endif // DEVICE_H