#ifndef INTERFACEEXCEPTIONS_H
#define INTERFACEEXCEPTIONS_H

#include <stdexcept>

struct DeviceIDOutOfBoundException : public std::exception {
    const char* what() const noexcept override {
        return "ID non presente fra i tuoi dispositivi domotici.";
    }
};

#endif