//Autore: Buso Riccardo

#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include "Timeline.h"       //Serve per regolare il flusso del tempo


class Device {
public:
    // Constructor

    Device(Timeline& timeline, int& t, std::string nome, int ID, double consumo,bool stato = false);

    // Destructor
    virtual ~Device();

    // Member functions
    virtual void turnOn();
    virtual void turnOff();
    virtual bool isOn() const;

    // Add more member functions and attributes as needed

protected:
    // attributi 
    std::string nome;    
    int ID; 
    int consumo; //consumo specifico del device
    bool on; //false spento, true accesso
    int tempoDiEsecuzione; //da quanto tempo è accesso, si resetta ogni volta che viene spento 
    Timeline timeLine ; 
    int *t; //tempo attuale in minuti dall'inizio della giornata



public:             //! Lo ho messo un attimo public che i getter mi servono in Interface.h
    // Getter and Setter for nome
    std::string getNome() const;
    void setNome(const std::string& nome); 

    // Getter and Setter for ID
    int getID() const;
    void setID(int ID);   

    // Getter and Setter for consumo
    int getConsumo() const;
    void setConsumo(int consumo); 

    // Getter and Setter for stato
    bool getStato() const;      //! Ripetuta, c'è già isOn, ma per coerenza con gli altri getter si mette lo stesso
    void setStato(bool stato);

    // Getter and Setter for tempoDiEsecuzione
    int getTempoDiEsecuzione() ;

     //funzione che calcoli il consumo del device in base al tempo di esecuzione
    virtual double getConsumoTot(); 
};

#endif // DEVICE_H