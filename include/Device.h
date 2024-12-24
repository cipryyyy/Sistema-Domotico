//Autore: Buso Riccardo

#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include "Timeline.h"       //Serve per regolare il flusso del tempo, sennò esce un macello

/*
! Leggimi please. 

Fammi dei getter per tutti i dati da mettere sul comando show (nome, consumo totale nella giornata, quante ore è stato acceso e blablabla)
Nel file timeline.h ho già scritto come funziona, come capire se un device è attivo in un dato momento e tutto il resto

Nel costruttore aggiungi un argomento passato per riferimento di timeline, in maniera tale che dall'interfaccia
creo una timeline, tutti i device si sincronizzano su di quella, così riesci a fare tutte le funzioni.
Non so se serva, ma se vuoi aggiungi anche un argomento per il tempo, passato per riferimento, così direttamente sono tutti collegati alla stessa variabile &t
Tipo:

CPDevice(timeline& timeline, int& t, std::string nome, int ID, double consumo, int durataCiclo, bool stato = false)

Il resto vedi te come fare, l'unica roba il tempo segnalo in minuti, quindi un'ora di esecuzione scrivi segna 60(minuti)
Per il resto l'interfaccia è già pronta praticamente, una volta che hai fatto i metodi ricontrollo che tutto funzioni correttamente
al massimo dai un occhiata al mio file, se ci sono problemi insultami.

Comunque sei proprio brutto <3
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