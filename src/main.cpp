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
        if (c == ' ') {                     // Se è uno spazio
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

    try {                                                               // Prova a estrarre ore e minuti, controllando che siano validi
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
              << "set <device> on                               - Accende dispositivo\n"
              << "set <device> off                              - Spegne dispositivo\n"
              << "set <device> on <start> [stop]                - Imposta timer dispositivo\n"
              << "kill <device>                                 - Spegni dispositivo forzatamente\n"
              << "rm <device>                                   - Rimuovi timer dispositivo\n"
              << "show                                          - Mostra gli stati di tutti i dispositivi\n"
              << "show <device>                                 - Mostra lo stato di un singolo dispositivo\n"
              << "install <device> <consumo> [true/false] [off] - Installa dispositivo\n"
              << "install <device> <consumo> <durata> [off]     - Installa dispositivo con ciclo programmato\n"
              << "unistall <device>                             - Disinstalla dispositivo\n"
              << "set time HH:MM                                - Imposta ora sistema\n"
              << "reset time                                    - Resetta ora a 00:00\n"
              << "reset timers                                  - Rimuovi tutti i timer\n"
              << "reset all                                     - Reset completo del sistema\n"
              << "exit                                          - Esci dal programma"
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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

            int arguments = tokens.size();

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

            // Debug: mostra i token inseriti dall'utente
            if (debug) {
                for (auto& token : tokens) {
                    std::cout << token << std::endl;
                }
            }

            // Commandi "show"
            if (tokens[0] == "show") {
                switch (arguments) {
                    case 1: 
                        system.show();
                        break;
                    case 2: 
                        try {
                            int deviceId = system.searchID(tokens[1]);
                            system.show(deviceId);
                        } catch (std::exception& e) {
                            std::cerr << "Errore: " << e.what() << std::endl;
                        }
                        break;
                    default:
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
                if (arguments < 2) {
                    throw std::invalid_argument("Utilizzo di set non valido. Usa 'help' per i comandi disponibili");
                    continue;
                }

                // Commando "set" per il tempo
                if (tokens[1] == "time") {
                    if (arguments != 3) {
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

                    if (arguments < 3) {
                        throw std::invalid_argument("Utilizzo di set non valido. Usa 'help' per i comandi disponibili");
                    }

                    if (tokens[2] == "on") {
                        if (arguments < 3) {
                            throw std::invalid_argument("Utilizzo di set on non valido. Usa 'help' per i comandi disponibili");
                        }

                        try {
                            if (arguments == 3) {
                                system.turnOn(deviceId);    // Accensione immediata
                            } else if (arguments > 3 && arguments <= 5) {
                                int start = 0;                              // Inizializzo start & end a 0
                                int end   = 0;                              // per evitare errori di compilazione
                                
                                if (arguments == 4) {                       // Accensione immediata
                                    std::cout << "Accensione programmata\n";
                                    start = parseTime(tokens[3]);           // Prendo orario di accesnsione
                                    system.turnOn(deviceId, start);         // Programmo l'accensione
                                } else if (arguments == 5) {                // Accensione programmata
                                    start = parseTime(tokens[3]);           // Prendo orario di accensione
                                    end   = parseTime(tokens[4]);           // Prendo orario di spegnimento
                                    system.turnOn(deviceId, start, end);    // Programmo il dispositivo
                                }
                            } else {
                                throw std::invalid_argument("Numero di argomenti non valido");
                            }
                        } catch (OverKWException& e) {
                            std::vector<int> turnOffSequence = system.turnOffSequence();    // Ottiene la sequenza di spegnimento
                            // provo a spegnere un dispositivo alla volta e riprovo l'accensione
                            while (!turnOffSequence.empty()) {
                                try {
                                    int device = turnOffSequence.back();
                                    system.turnOff(device);
                                    std::cout << std::endl;
                                    turnOffSequence.pop_back();        
                                    system.turnOn(deviceId);
                                    break;
                                } catch (OverKWException& e) { 
                                    if (turnOffSequence.empty()) {
                                        std::cerr << "Non ci sono KW a sufficienza. prepuzio.." << std::endl;
                                        break;
                                    }
                                    continue; 
                                }
                            }
                        } catch (TimerAlreadySetException& e) {
                            std::cerr << "Errore: " << e.what() << std::endl;
                        } catch (std::exception& e) {
                            std::cerr << "Errore: " << e.what() << std::endl;
                        }

                    } else if (tokens[2] == "off") {
                        if (arguments != 3) {
                            throw std::invalid_argument("Utilizzo di set off non valido. Usa 'help' per i comandi disponibili");
                        }
                        system.turnOff(deviceId);
                    } else {
                        throw std::invalid_argument("Comando non riconosciuto");
                    }
                } catch (std::exception& e) {
                    std::cerr << "Errore: " << e.what() << std::endl;
                }
                if (debug) system.debugKWs();
                if (debug) system.debugTOS();
                continue;
            }

            // Commando "kill"
            if (tokens[0] == "kill") {
                if (arguments != 2) {
                    throw std::invalid_argument("Utilizzo di kill non valido. Usa 'help' per i comandi disponibili");
                }
                try {
                    int deviceId = system.searchID(tokens[1]);
                    char choice;
                    std::cout << "Sei sicuro di voler spegnere il dispositivo " << tokens[1] << "? (y/n): ";
                    std::cin >> choice;
                    switch (choice) {
                        case 'y':
                            system.forceOff(deviceId);
                            break;
                        case 'n':
                            std::cout << "Operazione annullata\n";
                            break;
                        default:
                            break;
                    }
                } catch (std::exception& e) {
                    std::cerr << "Errore: " << e.what() << std::endl;
                }
                continue;
            }

            // Commando "rm"
            if (tokens[0] == "rm") {
                if (arguments != 2) {
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
                if (arguments != 2) {
                    throw std::invalid_argument("Utilizzo di reset non valido. Usa 'help' per i comandi disponibili");
                }
                if (tokens[1] == "time") {
                    system.resetTime();
                }
                else if (tokens[1] == "timers") {
                    system.resetTimers();
                }
                else if (tokens[1] == "all") {
                    system.resetAll();
                }
                else {
                    throw std::invalid_argument("Comando non riconosciuto. Usa 'help' per i comandi disponibili");
                }
                continue;
            }

            // Commandi "install"
            if (tokens[0] == "install") {
                if (arguments <= 3) {
                    throw std::invalid_argument("Utilizzo di install non valido. Usa 'help' per i comandi disponibili");
                }

                try {
                    if (arguments != 5) throw std::invalid_argument("Numero di argomenti non valido");  // Controllo numero di argomenti
                    if (tokens[4] != "on" && tokens[4] != "off") {                                      // Controllo validità stato
                        throw std::invalid_argument("Stato non valido. Usa 'on' o 'off'");
                    }

                    const std::string& name = tokens[1];            // Nome del dispositivo
                    double consumo = std::stod(tokens[2]);          // Consumo del dispositivo
                    bool isOn = tokens[4] == "on";                  // Stato del dispositivo

                    // In base al terzo argomento, installo un dispositivo manuale o programmato
                    if (tokens[3] == "true" || tokens[3] == "false") {      // Dispositivo manuale
                        bool autoTurnOff = (tokens[3] == "true");
                        system.installM(name, consumo, autoTurnOff, isOn);
                    } else {                                                // Dispositivo programmato
                        int durataCiclo = std::stoi(tokens[3]);
                        if (durataCiclo <= 0 && durataCiclo > 1439) {
                            throw std::invalid_argument("Durata ciclo non valida. Deve essere compresa tra 1 e 1439 minuti");
                        }
                        system.installCP(name, consumo, durataCiclo, isOn);
                    }

                } catch (std::exception& e) {
                    std::cerr << "Errore: " << e.what() << std::endl;
                }

                continue;
            }

            // Commando "uninstall"
            if (tokens[0] == "uninstall") {
                if (arguments != 2) {
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