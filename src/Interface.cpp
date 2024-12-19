#include "Interface.h"

Interface::Interface(float KW, int time = 0): maximumKW{KW}, t{time}//, devicesCP{new CicloPreImpostatoDevice[5]}, devicesM{new ManualDevice[5]}
{}

Interface::~Interface() {
    //delete[] devicesCP;
    //delete[] devicesM;
};

void Interface::turnOn() {

};

void Interface::turnOff() {

};

void Interface::removeTimer() {

};

void Interface::setTime() {

};

void Interface::resetTime() {

};

void Interface::show(int id = 0){
    
};