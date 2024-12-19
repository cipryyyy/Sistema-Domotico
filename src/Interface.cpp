#include "Interface.h"

Interface::Interface(float KW, int time): maximumKW{KW}, t{time}
{

}
void Interface::turnOn(int id) {        //! Provvisorio, controllare il comportamento CP/M
    if (id > 0 && id <= 5) {
        devicesCP[id-1].turnOn();
    } else if (id >= 6 && id <= 10) {
        devicesM[id-6].turnOn();
    } else {
        throw DeviceIDOutOfBoundException();
    }
};

void Interface::turnOff(int id) {        //! Provvisorio, controllare il comportamento CP/M
    if (id > 0 && id <= 5) {
        devicesCP[id-1].turnOff();
    } else if (id >= 6 && id <= 10) {
        devicesM[id-6].turnOff();
    } else {
        throw DeviceIDOutOfBoundException();
    }
};

void Interface::removeTimer(int id) {
    return;
};

void Interface::setTime(int time) {
    return;
};

void Interface::resetTime() {
    t = 0;
};

void Interface::show(int id){
    return;
};