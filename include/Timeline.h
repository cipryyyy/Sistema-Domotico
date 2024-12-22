/*
Autore: Cipriani Andrea
Questa libreria di supporto serve per generare una timeline
in grado di salvare gli eventi del sistema, con il relativo orario e tutti i metadati necessari.

Il vector time può contenere valori in un range 0 (00:00) - 1439 (23:59)
ovvero il tempo del giorno convertito in minuti, con ID che ha fatto la richiesta e la variazione di KW
Nel vettore 'e' viene data una breve descrizione dell'evento per il front end

I dispositivi hanno ID da 1 a 10, se un device in un dato momento sarà acceso il suo id sarà ID+10
Quindi se trovo 'ID 12' vuol dire che l'ID 2 è stato acceso, mentre se trovo 'ID 2' vuol dire che è stato spento.
Può sembrare una boiata però è utile e risparmia memoria per segnalare se un dispositivo è acceso o spento.
Per ottenere l'ID del device in sé o fai un IF oppure (ID-1)%10+1.

I metodi della libreria sono: 
* addEvent      aggiungere eventi
* print         per stampare a schermo gli eventi in un range start - end
* get[...]      ritorna un vettore con orari/eventi/id/kw in un range start - end
* clear         elimina tutti gli eventi
* forget        elimina gli eventi di un dispositivo in un lasso di tempo

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
    std::vector<int>         t;     //Salva il tempo
    std::vector<std::string> e;     //Testo che spiega cosa è successo
    std::vector<int>         d;     //ID del dispositivo che ha fatto la richiesta
    std::vector<double>      k;     //ΔKW

public:
    void addEvent(int time, std::string event, int ID, double KW) {
        if (time < 0 || time > 1439) {
            throw std::invalid_argument("Il tempo deve essere compreso tra 0 e 1439 minuti");
        }
        //Salvo tutti i dati
        t.push_back(time);
        e.push_back(event);
        d.push_back(ID);
        k.push_back(KW);
    }

    void print(int start = 0, int end = 1439) {                         //! Meglio usare i get, così si accede più facilmente ai dati di un device.
        if (end > start || start < 0 || end > 1439) {
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

    std::vector<int>& getTimes(int start = 0, int end = 1439) {
        if (end > start || start < 0 || end > 1439) {
            throw std::invalid_argument("Valori inseriti non validi");
        }
        std::vector<int> times;
        for (int i = 0; i < t.size(); i++) {
            if (t[i] >= start && t[i] <= end) {
                times.push_back(t[i]);
            }
        }
        return times;
    }

    std::vector<std::string>& getEvents(int start = 0, int end = 1439) {
        if (end > start || start < 0 || end > 1439) {
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

    std::vector<int>& getIDs(int start = 0, int end = 1439) {
        if (end > start || start < 0 || end > 1439) {
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

    std::vector<double>& getKWs(int start = 0, int end = 1439) {
        if (end > start || start < 0 || end > 1439) {
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

    void forget(int id, int begin = 0, int end = 1439) {
        for (int i = 0; i < t.size(); i++) {
            if (t[i] >= begin && (d[i] == id || d[i] == id+10)) {
                t.erase(t.begin() + i);
                e.erase(e.begin() + i);
                d.erase(d.begin() + i);
                k.erase(k.begin() + i);
            }
        }
    }

    void clear() {
        t.clear();
        e.clear();
        d.clear();
        k.clear();
    }
};

#endif