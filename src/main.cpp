//Autore: Giorgi Giacomo

/*
Tips (of my dick)
* Metti un argc nel main, così passi il numero di KW massimi, se non viene dato lo chiede come primo argomento
* set <device> <start> <stop> non lo riesco a fare partire, dimmi te se devo togliere la parte solo start per i dispositivi M
* set time non funziona con il formato 9:00, solo con 09:00, se hai tempo sistemalo, sennò non importa
* Ho messo una nuova eccezione nel caso si provi ad installare un dispositivo con uno stesso nome
* Nel caso si provi a disinstallare un dispositivo che non esiste non da nessun problema, non so perché
*/

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "Interface.h"

// Costanti per il sistema
const double MAX_POWER = 3.5;    // Potenza massina del sistema, in kW
const int MAX_DEVICES = 100;      // Numero massimo di dispositivi supportati

// Helper function per splittare una stringa in token
std::vector<std::string> tokenize(const std::string& cmd) {
    std::vector<std::string> tokens;
    std::stringstream ss(cmd);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Helper function per validare il formato del tempo
bool isValidTimeFormat(const std::string& time) {
    if (time.size() != 5) return false; // formattazione obbligatoria HH:MM
    if (time[2] != ':') return false;   // separatore tra ore e minuti

    try {
        int h = std::stoi(time.substr(0, 2));
        int m = std::stoi(time.substr(3, 2));
        return h >= 0 && h <= 23 && m >= 0 && m <= 59;
    } catch (std::invalid_argument& e) {
        return false;
    }
}

// Helper function per fare il parsing del tempo da HH:MM a minuti
int parseTime(const std::string& timeStr) {
    if (!isValidTimeFormat(timeStr)) {
        throw std::invalid_argument("Formato orario non valido");
    }

    int hours = std::stoi(timeStr.substr(0, 2));
    int minutes = std::stoi(timeStr.substr(3, 2));
    return hours * 60 + minutes;
}

// Helper function per mostrare il help message con i comandi disponibili
void displayHelp() {
    std::cout << "\nAvailable commands:\n"
              << "set <device> on                          - Accende dispositivo\n"
              << "set <device> off                         - Spegne dispositivo\n"
              << "set <device> <start> [stop]              - Imposta timer dispositivo\n"
              << "rm <device>                              - Rimuovi timer dispositivo\n"
              << "show                                     - Mostra gli stati di tutti i dispositivi\n"
              << "show <device>                            - Mostra lo stato di un singolo dispositivo\n"
              << "install <device> <consumo> [on]          - Installa dispositivo\n"
              << "install <device> <consumo> <durata> [on] - Installa dispositivo con ciclo programmato\n"
              << "unistall <device>                        - Disinstalla dispositivo\n"
              << "set time HH:MM                           - Imposta ora sistema\n"
              << "reset time                               - Resetta ora a 00:00\n"
              << "reset timers                             - Rimuovi tutti i timer\n"
              << "reset all                                - Reset completo del sistema\n"
              << "exit                                     - Esci dal programma"
              << std::endl;
}

int main() {
    // Inizializzo il sistema
    Interface system(MAX_POWER, true, MAX_DEVICES);
    std::string command;
    
    // Apro file di log
    std::ofstream logFile("system.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Errore nell'apertura del file di log\n";
        return 1;
    }
    
    std::cout << "Smart Home Energy Management System\n";
    std::cout << "Type 'help' for available commands\n";

    while (true) {
        std::cout << "\n>> ";
        getline(std::cin, command);

        // Loggo il comando
        logFile << "[" << "] User input: " << command << std::endl;

        try {
            auto tokens = tokenize(command);
            if (tokens.empty()) continue;

            // Display help
            if(tokens[0] == "help" || tokens[0] == "h") {
                displayHelp();
                continue;
            }

            // Commando uscita
            if (tokens[0] == "exit" || tokens[0] == "q") {
                logFile << "Exiting program\n";
                break;
            }

            // Commandi "show"
            if (tokens[0] == "show") {
                if (tokens.size() == 1) {
                    system.show();
                } else if (tokens.size() == 2) {
                    try {
                        int deviceId = system.searchID(tokens[1]);
                        system.show(deviceId);
                    } catch (std::exception& e) {
                        std::cerr << "Errore: " << e.what() << std::endl;
                    }
                } else {
                    throw std::invalid_argument("Numero di argomenti non valido");
                }
                continue;
            }

            // Commandi "set"
            if (tokens[0] == "set") {
                if (tokens.size() < 2) {
                    throw std::invalid_argument("Utilizzo di set non valido. Usa 'help' per i comandi disponibili");
                    continue;
                }

                // Commando "set" per il tempo
                if (tokens[1] == "time") {
                    if (tokens.size() != 3) {
                        throw std::invalid_argument("Utilizzo di set time non valido. Usa 'help' per i comandi disponibili");
                    }
                    try {
                        int newTime = parseTime(tokens[2]);
                        system.setTime(newTime);
                    } catch (std::exception& e) {
                        std::cerr << "Errore: " << e.what() << std::endl;
                    }
                    continue;
                }

                // Commandi per dispositivi
                try {
                    int deviceId = system.searchID(tokens[1]);

                    if (tokens.size() < 3) {
                        throw std::invalid_argument("Utilizzo di set non valido. Usa 'help' per i comandi disponibili");
                    }

                    if (tokens[2] == "on") {
                        switch (tokens.size()) {
                            case 3:
                                system.turnOn(deviceId);
                                break;
                            case 4:
                                system.turnOn(deviceId, parseTime(tokens[3]));
                                break;
                            case 5:
                                system.turnOn(deviceId, parseTime(tokens[3]), parseTime(tokens[4]));
                                break;
                            default:
                                throw std::invalid_argument("Numero di argomenti non valido");
                        }
                    } else if (tokens[2] == "off") {
                        system.turnOff(deviceId);
                    } else {
                        throw std::invalid_argument("Comando non riconosciuto");
                    }
                } catch (std::exception& e) {
                    std::cerr << "Errore: " << e.what() << std::endl;
                }
                continue;
            }

            // Commando "rm"
            if (tokens[0] == "rm") {
                if (tokens.size() != 2) {
                    throw std::invalid_argument("Utilizzo di rm non valido. Usa 'help' per i comandi disponibili");
                }
                try {
                    int deviceId = system.searchID(tokens[1]);
                    system.removeTimer(deviceId);
                } catch (std::exception& e) {
                    std::cerr << "Errore: " << e.what() << std::endl;
                }
                continue;
            }

            // Commando "reset"
            if (tokens[0] == "reset") {
                if (tokens.size() != 2) {
                    throw std::invalid_argument("Utilizzo di reset non valido. Usa 'help' per i comandi disponibili");
                }
                if (tokens[1] == "time") {
                    system.resetTime();
                }
                else if (tokens[1] == "timers") {
                    // system.resetTimers();
                }
                else if (tokens[1] == "all") {
                    // system.resetAll();
                }
                else {
                    throw std::invalid_argument("Comando non riconosciuto. Usa 'help' per i comandi disponibili");
                }
                continue;
            }

            // Commandi "install"
            if (tokens[0] == "install") {
                if (tokens.size() <= 3) {
                    throw std::invalid_argument("Utilizzo di install non valido. Usa 'help' per i comandi disponibili");
                }

                try {
                    bool isOn = false;
                    switch (tokens.size()) {
                        case 4:
                            if (tokens[2] == "on") {
                                isOn = true;
                            }
                            system.installM(tokens[1], std::stod(tokens[2]), isOn);
                            break;
                        case 5:
                            if (tokens[3] == "on") {
                                isOn = true;
                            }
                            system.installCP(tokens[1], std::stod(tokens[2]), std::stoi(tokens[3]), isOn);
                            break;
                        default:
                            throw std::invalid_argument("Numero di argomenti non valido");
                            break;
                    }
                } catch (std::exception& e) {
                    std::cerr << "Errore: " << e.what() << std::endl;
                }

                continue;
            }
        } catch (std::exception& e) {
            std::cerr << "Errore: " << e.what() << std::endl;
        }
    }

    logFile.close();
    return 0;
}