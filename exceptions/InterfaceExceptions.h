/*
Autore: Cipriani Andrea

Eccezioni che possono essere lanciate dalla classe Interface
*/
#ifndef INTERFACEEXCEPTIONS_H
#define INTERFACEEXCEPTIONS_H

#include <stdexcept>

//ID invalido:
struct DeviceIDOutOfBoundException : public std::exception {
    const char* what() const noexcept override {
        return "ID non presente fra i tuoi dispositivi domotici.";
    }
};

//Spegnimento di un deviceCP durante il ciclo:
struct CPIllegalInstructionException : public std::exception {
    const char* what() const noexcept override {
        return "Impossibile spegnere i dispositivi a ciclo predefinito prima del termine";
    }
};

//Superamento soglia KW
struct OverKWException : public std::exception {
    const char* what() const noexcept override {
        return "Impossibile avviare il dispositivo, KW insufficienti";
    }
};

//Richiesta di variazione temporale negativa
struct NotATimeMachineException : public std::exception {
    const char* what() const noexcept override {
        return "Impossibile variare il tempo in negativo";
    }
};

//Richiesta programmazione duplicata
struct TimerAlreadySetException : public std::exception {
    const char* what() const noexcept override {
        return "Programmazione già avviata per questo dipositivo nella fascia temporale";
    }
};

#endif