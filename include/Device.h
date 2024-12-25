//Autore: Buso Riccardo

#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include "Timeline.h"       //Serve per regolare il flusso del tempo, sennò esce un macello

/*
! Leggimi please. 

Migliora i costruttori, che per ora non CPDevice e ManualDevice sono trattati come classi indipendenti (che l'uomo sesso
valuta anche ste boiate sicuro), tanto MDevice rimane identico, CPDevice chiama il costruttore device ed aggiunge il tempo
di esecuzione, nulla di più

Aggiungi il passaggio di una timeline per riferimento e del tempo t, sempre per riferimento. Tipo:

Device(Timeline& timeline, int& t, .......) : ......

Questo perché così hai accesso a tutte le programmazioni fatte da interface e riesci a calcolare il tempo in cui
un device rimane acceso, se al tempo t un device è acceso e tutto il resto. Leggiti un attimo come ho strutturato la classe,
ad inizio del file c'è un commento in cui spiego tutto comunque, l'unico metodo che ti interessa sono i get vari, il resto
sono tutti metodi di interface.

Per il resto i metodi sono tutti ok, a parte getStato che non serve a parer mio, i file .cpp te li ho già preparati con
gli #include degli header, se trovi dei metodi scritti dentro vai tra che non servono ad un cazzo, mi servivano solo
perché sennò CMake rompe le palle con il linking.

Ricordati che lo scorrere del tempo è in minuti, quindi tempo di esecuzione di 1 ora ritorna come valore: 60.

Ciao TVB.
*/

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
    void setNome(const std::string& nome); 

    // Getter and Setter for ID
    int getID() const;
    void setID(int ID);   

    // Getter and Setter for consumo
    int getConsumo() const;
    void setConsumo(int consumo); 

    // Getter and Setter for stato
    bool getStato() const;      //! Ripetuta, c'è già isOn
    void setStato(bool stato);

    // Getter and Setter for tempoDiEsecuzione
    int getTempoDiEsecuzione() const;
    void setTempoDiEsecuzione(int tempoDiEsecuzione);
};

#endif // DEVICE_H