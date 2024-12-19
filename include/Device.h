//Autore: Buso Riccardo

#ifndef DEVICE_H
#define DEVICE_H

#include <string>

class Device {
public:
    // Constructor
    Device();

    // Destructor
    ~Device();

    // Member functions
    void turnOn();
    void turnOff();
    bool isOn() const;

    // Add more member functions and attributes as needed

protected:
    // attributi 
    
    std::string nome;
    int ID; 
    int consumo; //consumo specifico del device
    bool stato; //0 spento 1 accesso
    int tempoDiEsecuzione; //da quanto tempo è accesso


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
    bool getStato() const;
    void setStato(bool stato);

    // Getter and Setter for tempoDiEsecuzione
    int getTempoDiEsecuzione() const;
    void setTempoDiEsecuzione(int tempoDiEsecuzione);
};

#endif // DEVICE_H