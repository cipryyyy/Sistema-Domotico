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
    bool stato; //false spento, true accesso
    int tempoDiEsecuzione; //da quanto tempo è accesso

public:             //! Lo ho messo un attimo public che i getter mi servono in Interface.h
    // Getter and Setter for nome
    std::string getNome() const;
    void setNome(const std::string& nome);  //! Non serve

    // Getter and Setter for ID
    int getID() const;
    void setID(int ID);         //! Non serve

    // Getter and Setter for consumo
    int getConsumo() const;
    void setConsumo(int consumo);   //! Non serve

    // Getter and Setter for stato
    bool getStato() const;      //! ci sono già turnOn, turnOff e isOn, non penso servano
    void setStato(bool stato);

    // Getter and Setter for tempoDiEsecuzione
    int getTempoDiEsecuzione() const;
    void setTempoDiEsecuzione(int tempoDiEsecuzione);
};

#endif // DEVICE_H