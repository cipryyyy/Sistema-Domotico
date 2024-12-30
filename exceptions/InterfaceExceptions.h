/*
Autore: Cipriani Andrea

Eccezioni che possono essere lanciate dalla classe Interface, per ore sono tutte derivate dalla exception standard
Appena ho tempo e voglia magari metto un errore più specifico.
*/
#ifndef INTERFACEEXCEPTIONS_H
#define INTERFACEEXCEPTIONS_H

#include <stdexcept>

//ID invalido:
//! Lanciata da: turnOn(id), turnOn(id, start), turnOn(id, start, end), turnOff(id), show(id), removeTimer(id)
struct DeviceIDOutOfBoundException : public std::exception {
    const char* what() const noexcept override {
        return "ID non presente fra i tuoi dispositivi domotici.";
    }
};

//Spegnimento di un deviceCP durante il ciclo:
//! Lanciata da: turnOn(id, start, end), turnOff(id)
struct CPIllegalInstructionException : public std::exception {
    const char* what() const noexcept override {
        return "Impossibile spegnere i dispositivi a ciclo predefinito prima del termine.";
    }
};

//Superamento soglia KW:
//! Lanciata da: turnOn(id), turnOn(id, start), turnOn(id, start, end)
struct OverKWException : public std::exception {
    const char* what() const noexcept override {
        return "Impossibile avviare il dispositivo, KW insufficienti.";
    }
};

//Richiesta programmazione duplicata:
//! Lanciata da: turnOn(id, start), turnOn(id, start, end)
struct TimerAlreadySetException : public std::exception {
    const char* what() const noexcept override {
        return "Programmazione già avviata per questo dipositivo nella fascia temporale.";
    }
};

//Time oltre ai limiti:
//! Lanciata da: turnOn(id, start), turnOn(id, start, end), setTime(time)
struct InvalidTimeException : public std::exception {
    const char* what() const noexcept override {
        return "L'orario deve essere tra le 00:00 e le 23:59 (0-1439).";
    }
};

//Numero di devices superiore al limite
//! Lanciata da: installM, installCP
struct DeviceLimitException : public std::exception {
    const char* what() const noexcept override {
        return "Hai raggiunto il numero massimo di devices su questa interfaccia.";
    }
};

//Richiesta di variazione temporale negativa:
//! Lanciata da: searchID
struct NameNotFoundException : public std::exception {
    const char* what() const noexcept override {
        return "Nome non presente tra i dispositivi.";
    }
};


//Richiesta di variazione temporale negativa:
//! Lanciata da: turnOn(id, start), turnOn(id, start, end), setTime(time)
struct NotATimeMachineException : public std::exception {
    const char* what() const noexcept override {
        return "Impossibile variare il tempo in negativo.";
    }
};

//Installazione dispositivo con lo stesso nome:
//! Lanciata da: installCP, installM
struct DuplicateDeviceException : public std::exception {
    const char* what() const noexcept override {
        return "E' già presente un device con questo nome.";
    }
};

//Richiesta accensione device già acceso:
//! Lanciata da: turnOn
struct DeviceAlreadyOnException : public std::exception {
    const char* what() const noexcept override {
        return "Il dispositivo è già acceso.";
    }
};

//Non ho ancora finito il metodo:
//! Lanciata da: i metodi che non ho finito
struct Error404FunctionNotFound : public std::exception {
    const char* what() const noexcept override {
        return "Passa più tardi, non ho ancora finito di fare questa funzione.";
    }
};

#endif