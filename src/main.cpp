//Autore: Giorgi Giacomo

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>
#include "Interface.h"

// Costanti di default per il sistema
bool debug = false;
const double MAX_POWER = 3.5;       // Potenza massina del sistema, in kW
const int MAX_DEVICES = 100;        // Numero massimo di dispositivi supportati

// Helper function per splittare una stringa in token
std::vector<std::string> tokenize(const std::string& cmd) {
    std::vector<std::string> tokens;        // Vector di stringhe per i token
    std::string token;                      // Stringa temporanea per i token
    bool inQuotes = false;                  // Flag per le virgolette

    for (const char& c : cmd) {             // Controllo ogni carattere della stringa
        if (c == '"') {                     // Se trovo una virgoletta
            inQuotes = !inQuotes;           // Cambio il flag
            continue;
        }

        if (c == ' ' && !inQuotes) {        // Se trovo uno spazio e non sono tra virgolette
            if (!token.empty()) {           // Se il token non è vuoto
                tokens.push_back(token);    // Aggiungo il token al vector
                token.clear();              // Svuoto il token
            }
        } else {                            // Se non è uno spazio o sono tra virgolette
            token += c;                     // Aggiungo il carattere al token
        }
    }

    if (!token.empty()) {                   // Se alla fine del ciclo il token non è vuoto
        tokens.push_back(token);            // Aggiungo il token al vector
    }
    
    return tokens;
}

// Helper function per validare il formato del tempo
bool isValidTimeFormat(const std::string& time) {
    if (time.size() < 4 || time.size() > 5) return false; // Verifica lunghezza minima e massima della stringa
    if (time.size() == 5 && time[2] != ':') return false; // Verifica il delimitatore
    if (time.size() == 4 && time[1] != ':') return false; // Verifica il delimitatore

    try {                                                           // Prova a estrarre ore e minuti, controllando che siano validi
        int h = std::stoi(time.substr(0, time.size() == 5 ? 2 : 1));    // Ore
        int m = std::stoi(time.substr(time.size() == 5 ? 3 : 2));       // Minuti
        return h >= 0 && h <= 23 && m >= 0 && m <= 59;
    } catch (const std::exception& e) {                             // Ritorna false se ci sono errori di parsing
        std::cerr << "Errore: " << e.what() << std::endl;
        return false; 
    }
}

// Helper function per convertire tempo in minuti
int parseTime(const std::string& timeStr) {
    // Controlla che il formato dell'orario sia valido
    if (!isValidTimeFormat(timeStr)) {
        throw std::invalid_argument("Formato orario non valido");
    }

    // Determina la posizione di ore e minuti
    bool isShortFormat = timeStr.size() == 4;                        // Controlla se è in formato breve (H:MM)
    int hours = std::stoi(timeStr.substr(0, isShortFormat ? 1 : 2)); // Ore
    int minutes = std::stoi(timeStr.substr(isShortFormat ? 2 : 3));  // Minuti

    // Restituisci il totale in minuti
    return hours * 60 + minutes;
}

// Helper function per mostrare il help message con i comandi disponibili
void displayHelp() {
    std::cout << "\nFunzioni disponibili:\n"
              << "set <device> on                          - Accende dispositivo\n"
              << "set <device> off                         - Spegne dispositivo\n"
              << "set <device> on/off <start> [stop]       - Imposta timer dispositivo\n"
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

// Helper function per controllare se il valore della potenza massima è valido
bool isValidPower(double power) {
    return power > 0 && power <= 10;
}

int main(int argc, char* argv[]) {

    // Inizializzo la potenza massima del sistema
    double power;       // Variabile potenza massima del sistema
    if (argc > 1) {     // Se è stato passato un argomento
        try {
            power = std::stod(argv[1]);  // Converto l'argomento in double
            if (!isValidPower(power)) {  // Controllo che la potenza inserita sia valida
                throw std::invalid_argument("La potenza deve essere positiva e massimo 10 kW");
            }
        } catch (const std::exception& e) {
            std::cerr << "Errore: " << e.what() << "\n";
            std::cerr << "Impostato limite massimo di potenza al valore predefinito di " << MAX_POWER << " kW\n";
            power = MAX_POWER;
        }
    } else {            // Se non è stato passato alcun argomento
        try {
            std::cout << "Inserire la potenza massima del sistema (kW): "; // Chiedo all'utente di inserire la potenza massima del sistema
            std::cin >> power;                                             // Salvo il valore inserito in power

            // Controllo se l'input è un numero
            if (std::cin.fail()) {
                std::cin.clear();                                                   // Resetta lo stato del cin
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignora input non valido
                throw std::invalid_argument("Valore non numerico");                 // Solleva eccezione
            }

            if (!isValidPower(power)) {  // Controllo che la potenza inserita sia valida
                throw std::invalid_argument("La potenza deve essere positiva e massimo 10 kW");
            }
        } catch (const std::exception& e) {
            std::cerr << "Errore: " << e.what() << "\n";
            std::cerr << "Utilizzo il valore predefinito " << MAX_POWER << " kW\n";
            power = MAX_POWER;
        }
    }

    // Inizializzo il sistema
    Interface system(power, true, MAX_DEVICES);
    if (debug) std::cout << "Avvio con " << power << " KW e " << MAX_DEVICES << " device." << std::endl; 
    std::string command;
    
    // Apro file di log
    std::ofstream logFile("system.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Errore nell'apertura del file di log\n";
        return 1;
    }
    
    std::cout << "Smart Home Energy Management System\n";
    std::cout << "-------------------------------------------------\n";
    std::cout << "Scrivi 'help' per vedere le funzioni disponibili\n";
    std::cout << "Scrivi 'exit' per uscire dal programma\n";
    std::cout << "-------------------------------------------------" << std::endl;

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
                if (debug) {
                    std::cout << "Contatori: ";
                    system.debugCounters();
                    std::cout << std::endl;
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
                    if (debug) {
                        std::cout << "KW: ";
                        system.debugKWs();
                        std::cout << std::endl << "Time: ";
                        system.debugTime();
                        std::cout << std::endl;
                    }
                    continue;
                }

                // Commandi per dispositivi
                try {
                    if (debug) std::cout << "Ricerca di: " << tokens[1] << std::endl;
                    int deviceId = system.searchID(tokens[1]);
                    if (debug) std::cout << "ID TROVATO: " << deviceId << std::endl;

                    if (tokens.size() < 3) {
                        throw std::invalid_argument("Utilizzo di set non valido. Usa 'help' per i comandi disponibili");
                    }

                    if (tokens[2] == "on") {
                        switch (tokens.size()) {
                            case 3:
                                if (debug) std::cout << "turnOn manuale" << std::endl;
                                system.turnOn(deviceId);
                                break;
                            case 4:
                                if (debug) std::cout << "turnOn con start" << std::endl;
                                system.turnOn(deviceId, parseTime(tokens[3]));
                                break;
                            case 5:
                                if (debug) std::cout << "Routine" << std::endl;
                                system.turnOn(deviceId, parseTime(tokens[3]), parseTime(tokens[4]));
                                break;
                            default:
                                throw std::invalid_argument("Numero di argomenti non valido");
                        }
                    } else if (tokens[2] == "off") {
                        if (debug) std::cout << "Spegnimento" << std::endl;
                        system.turnOff(deviceId);
                    } else {
                        throw std::invalid_argument("Comando non riconosciuto");
                    }
                } catch (std::exception& e) {
                    std::cerr << "Errore: " << e.what() << std::endl;
                }
                if (debug) system.debugKWs();
                continue;
            }

            // Commando "rm"
            if (tokens[0] == "rm") {
                if (tokens.size() != 2) {
                    throw std::invalid_argument("Utilizzo di rm non valido. Usa 'help' per i comandi disponibili");
                }
                try {
                    if (debug) std::cout << "Ricerca di: " << tokens[1] << std::endl;
                    int deviceId = system.searchID(tokens[1]);
                    if (debug) std::cout << "ID TROVATO: " << deviceId << std::endl;
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
                    system.resetTimers();
                }
                else if (tokens[1] == "all") {
                    system.resetTime();
                    system.resetTimers();
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

            // Commando "uninstall"
            if (tokens[0] == "uninstall") {
                if (tokens.size() != 2) {
                    throw std::invalid_argument("Utilizzo di uninstall non valido. Usa 'help' per i comandi disponibili");
                }
                try {
                    int deviceId = system.searchID(tokens[1]);
                    system.uninstall(deviceId);
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