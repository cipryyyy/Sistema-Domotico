/*
Autori: Cipriani Andrea, Giorgi Giacomo
*/

#ifndef TIMELINE_H
#define TIMELINE_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>

class Timeline{

private:
    int range;
    std::vector<int>         t;     //Salva il tempo
    std::vector<std::string> e;     //Testo che spiega cosa è successo
    std::vector<int>         d;     //ID del dispositivo che ha fatto la richiesta
    std::vector<double>      k;     //ΔKW
    std::vector<bool>        r;    //Flag per sapere se è comando di routine

public:
    Timeline(int MaxID = 0) : range{MaxID} {}
    void addEvent(int time, std::string event, int ID, double KW, bool routine) {         //Aggiunge un evento
        if (time < 0 || time > 1439) {
            throw std::invalid_argument("Il tempo deve essere compreso tra 0 e 1439 minuti"); //24 h 
        }
        for (int i = 0; i < d.size(); i++) {
            if ((std::abs(ID - d[i]) == 100) && t[i] == time) {
                t.erase(t.begin() + i);
                e.erase(e.begin() + i);
                d.erase(d.begin() + i);
                k.erase(k.begin() + i);
                r.erase(r.begin() + i);
                return; 
            }
        }
        t.push_back(time);
        e.push_back(event);
        d.push_back(ID);
        k.push_back(KW);
        r.push_back(routine);
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
        return std::move(events);
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
        return std::move(IDs);
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
        return std::move(KWs);
    }

    void forget(int id, int begin = 0, int end = 1439) {        //Elimina gli eventi di un device in un lasso di tempo
        for (int i = 0; i < t.size(); i++) {
            if (begin == 0 && end == 1439) {        // Se devo cancellare da inizio a fine
                clear();
                return;
            }
            if (t[i] >= begin && t[i] <= end && (d[i] == id || d[i] == (id+range))) {
                t.erase(t.begin() + i);
                e.erase(e.begin() + i);
                d.erase(d.begin() + i);
                k.erase(k.begin() + i);
                r.erase(r.begin() + i);
                --i;                        //Siccome rimuovo un elemento, devo tornare indietro di 1
            }
        }
    }

    void clear() {          //Cancella tutto dalla timeline
        t.clear();
        e.clear();
        d.clear();
        k.clear();
        r.clear();
    }

    void setRange(int mid) {
        range = mid;
    }
    int getRange() const noexcept {
        return range;
    }

    void removeNonRoutines(int begin = 0, int end = 1439) {
        for (int i = 0; i < r.size(); i++) {
            if (t[i] >= begin && t[i] <= end && !r[i]) {
                t.erase(t.begin() + i);
                e.erase(e.begin() + i);
                d.erase(d.begin() + i);
                k.erase(k.begin() + i);
                r.erase(r.begin() + i);
                --i;
            }
        }
    }
};
#endif