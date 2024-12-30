/*
Autori: Cipriani Andrea, Giorgi Giacomo

Questa libreria di supporto serve per generare una timeline
in grado di salvare gli eventi del sistema, con il relativo orario e tutti i metadati necessari.

Il vector time può contenere valori in un range 0 (00:00) - 1439 (23:59)
ovvero il tempo del giorno convertito in minuti, con ID che ha fatto la richiesta e la variazione di KW
Nel vettore 'e' viene data una breve descrizione dell'evento per il front end.

L'ID salvato è uguale a quello del dispositivo se viene spento, mentre sarà ID+1024 se viene acceso
Questo perché il sistema supporta fino a 1024 dispositivi
il numero dei device è arbitrario, comunque numero difficile da raggiungere e in caso modificabile in inizializzazione.

I metodi della libreria sono: 
* addEvent      Aggiungere eventi
* print         Per stampare a schermo gli eventi in un range start - end
* get[...]      Ritorna un vettore con orari/eventi/id/kw in un range start - end
* clear         Elimina tutti gli eventi
* forget        Elimina gli eventi di un dispositivo in un lasso di tempo
* setRange      Imposta il valore massimo degli ID per segnalare quando è acceso o spento il device
* getRange      Ottieni il valore massimo degli ID per segnalare quando è acceso o spento il device

L'unico errore è per l'input errato, lanciato con invalid_argument, in realtà questa eccezione
è già coperta in interface.h, ma non si sa mai.
*/

#ifndef TIMELINE_H
#define TIMELINE_H

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

class Timeline{

private:
    int range;                      
    std::vector<int>         t;     //Salva il tempo
    std::vector<std::string> e;     //Testo che spiega cosa è successo
    std::vector<int>         d;     //ID del dispositivo che ha fatto la richiesta
    std::vector<double>      k;     //ΔKW

public:
    Timeline(int MaxID = 0) : range{MaxID} {}
    void addEvent(int time, std::string event, int ID, double KW) {         //Aggiunge un evento
        if (time < 0 || time > 1439) {
            throw std::invalid_argument("Il tempo deve essere compreso tra 0 e 1439 minuti"); //24 h 
        }
        //Salvo tutti i dati
        t.push_back(time);
        e.push_back(event);
        d.push_back(ID);
        k.push_back(KW);
    }

    void print(int start = 0, int end = 1439) {                         //! Meglio usare i get, così si accede più facilmente ai dati di un device.
        if (end < start || start < 0 || end > 1439) {
            throw std::invalid_argument("Valori inseriti non validi");
        }
        for (int i = 0; i < t.size(); i++) {
            if (t[i] >= start && t[i] <= end) {
                std::string hour = std::to_string(t[i]/60);
                std::string minute = std::to_string(t[i] % 60);

                std::string hr24time = (hour.size() == 1 ? '0' + hour : hour) + ":" + (minute.size() == 1 ? '0' + minute : minute);
                std::cout << "[" << hr24time << "]: " << e[i] << std::endl;
            }
        }
    }

    std::vector<int> getTimes(int start = 0, int end = 1439) {      //Ritorna i timestamp in un lasso di tempo
        if (end < start || start < 0 || end > 1439) {
            throw std::invalid_argument("Valori inseriti non validi");
        }
        std::vector<int> times;
        for (int i = 0; i < t.size(); i++) {
            if (t[i] >= start && t[i] <= end) {
                times.push_back(t[i]);
            }
        }
        return std::move(times);
    }

    std::vector<std::string> getEvents(int start = 0, int end = 1439) {     //Ritorna gli eventi in un lasso di tempo
        if (end < start || start < 0 || end > 1439) {
            throw std::invalid_argument("Valori inseriti non validi");
        }
        std::vector<std::string> events;
        for (int i = 0; i < e.size(); i++) {
            if (t[i] >= start && t[i] <= end) {
                events.push_back(e[i]);
            }
        }
        return events;
    }

    std::vector<int> getIDs(int start = 0, int end = 1439) {        //Ritorna gli id agenti in un lasso di tempo
        if (end < start || start < 0 || end > 1439) {
            throw std::invalid_argument("Valori inseriti non validi");
        }
        std::vector<int> IDs;
        for (int i = 0; i < e.size(); i++) {
            if (t[i] >= start && t[i] <= end) {
                IDs.push_back(d[i]);
            }
        }
        return IDs;
    }

    std::vector<double> getKWs(int start = 0, int end = 1439) {     //Ritorna le variazioni in un lasso di tempo
        if (end < start || start < 0 || end > 1439) {
            throw std::invalid_argument("Valori inseriti non validi");
        }
        std::vector<double> KWs;
        for (int i = 0; i < e.size(); i++) {
            if (t[i] >= start && t[i] <= end) {
                KWs.push_back(k[i]);
            }
        }
        return KWs;
    }


    void forget(int id, int begin = 0, int end = 1439) {        //Elimina gli eventi di un device in un lasso di tempo
        for (int i = 0; i < t.size(); i++) {
            if (t[i] >= begin && (d[i] == id || d[i] == (id+range))) {
                t.erase(t.begin() + i);
                e.erase(e.begin() + i);
                d.erase(d.begin() + i);
                k.erase(k.begin() + i);
                --i;                        //Siccome rimuovo un elemento, devo tornare indietro di 1
            }
        }
    }

    void clear() {          //Cancella tutto dalla timeline
        t.clear();
        e.clear();
        d.clear();
        k.clear();
    }

    void setRange(int mid) {
        range = mid;
    }
    int getRange() const noexcept {
        return range;
    }
};

#endif