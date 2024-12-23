#include "Device.h"

//Sono tutte dummy, non fanno una sega
//Le ho scritte solo per vedere se almeno compila la parte interface

Device::Device() {

}
Device::~Device() {

}
void Device::turnOn() {
    return;
};
void Device::turnOff() {
    return;
};
std::string Device::getNome() const{
    return "";
}
int Device::getConsumo() const {
    return 0;
}
bool Device::isOn() const {
    return false;
}
int Device::getID() const {
    return 0;
}
int Device::getTempoDiEsecuzione() const {
    return 0;
}