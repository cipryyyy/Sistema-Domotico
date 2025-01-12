//Autore: Giacomo Giorgi
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

#ifndef LOGGER_H
class Logger {
  private:
    std::ofstream logFile;    // File di log
    std::string logFileName;  // Nome del file di log
    bool debugMode;           // Modalità debug

    // Funzione per ottenere il timestamp corrente
    std::string getTimestamp(bool isFileTitle = false) {
      std::time_t now = std::time(nullptr);
      std::tm* localTime = std::localtime(&now);
      char buffer[20];
      if (isFileTitle) {
        std::strftime(buffer, sizeof(buffer), "%Y%m%d-%H%M", localTime);
      } else {
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
      }
      return std::string(buffer);
    }

  public: 
    enum LogLevel { DEBUG, USERINPUT, EVENT, ERROR };                                 // Livelli di log
    
    Logger(bool debug = false)                                                        // Costruttore
      : debugMode(debug) {
          logFileName = "../logs/log_" + getTimestamp(true) + ".txt";                 // Crea il nome del file di log
          logFile.open(logFileName, std::ios::app);                                   // Apre il file di log in modalità append
          if (!logFile.is_open()) {                                                   // Se il file di log non è aperto
            std::cerr << "Impossibile aprire file: " << logFileName << std::endl;     // Stampa un messaggio di errore
            throw std::ios_base::failure("Impossibile aprire file di log");           // E lancia un'eccezione
          }
          log(LogLevel::EVENT, std::string("Applicazione avviata in modalità ") + (debug ? "debug" : "normale"));
    }

    // Costruttori e operatori di assegnazione disabilitati
    Logger(const Logger&) = delete;                                               // Costruttore di copia disabilitato
    Logger& operator=(const Logger&) = delete;                                    // Operatore di assegnazione disabilitato

    Logger(Logger&&) = default;                                                   // Costruttore di spostamento
    Logger& operator=(Logger&&) = default;                                        // Operatore di assegnazione di spostamento

    ~Logger() {                                                                   // Distruttore
        if (logFile.is_open()) {                                                  // Se il file di log è aperto
            logFile.close();                                                      // Chiude il file di log
        }
    }

    void log(LogLevel level, const std::string& message) {                        // Funzione per scrivere un messaggio di log
      if (!logFile.is_open() || (level == DEBUG && !debugMode)) return;           // Se il file di log non è aperto o il livello di log è DEBUG e la modalità debug è disattivata, esce

      static const char* levelStr[] = { "DEBUG", "USERINPUT", "EVENT", "ERROR" }; // Array di stringhe per i livelli di log
      std::ostringstream logEntry;                                                // Stream per il messaggio di log
 
      logEntry << "[" << getTimestamp() << "] "                                   // Aggiunge il timestamp al messaggio di log
               << "[" << levelStr[level] << "] "                                  // Aggiunge il livello di log al messaggio di log
               << message << std::endl;                                           // Aggiunge il messaggio al messaggio di log
 
      logFile << logEntry.str();                                                  // Scrive il messaggio sul file di log
      logFile.flush();                                                            // Svuota il buffer del file di log
    } 
 
    void close() {                                                                // Funzione per chiudere il file di log
      if (logFile.is_open()) {                                                    // Se il file di log è aperto
        logFile.close();                                                          // Chiude il file di log
      }
    }
};

#define LOGGER_H
#endif