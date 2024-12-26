#include "Device.h"

//Sono tutte dummy, non fanno una sega
//Le ho scritte solo per vedere se almeno compila la parte interface

Device::Device() : nome(""), ID(0), consumo(0), stato(false), tempoDiEsecuzione(0) {}

Device::~Device() {}

void Device::turnOn() {
    stato = true;
    tempoDiEsecuzione = 0;
}

void Device::turnOff() {
    stato = false;
}

bool Device::isOn() const {
    return stato;
}

std::string Device::getNome() const {
    return nome;
}

void Device::setNome(const std::string& newNome) {
    nome = newNome;
}

int Device::getID() const {
    return ID;
}

void Device::setID(int newID) {
    ID = newID;
}

int Device::getConsumo() const {
    return consumo;
}

void Device::setConsumo(int newConsumo) {
    consumo = newConsumo;
}

void Device::setStato(bool newStato) {
    stato = newStato;
}

int Device::getTempoDiEsecuzione() const {
    return tempoDiEsecuzione;
}

void Device::setTempoDiEsecuzione(int newTempoDiEsecuzione) {
    tempoDiEsecuzione = newTempoDiEsecuzione;
}