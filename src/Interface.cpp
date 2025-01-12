#include "Interface.h"

//Funzioni helper
int Interface::CPscan(int id) const noexcept {		    //Indice e appartenenza di un device alla categoria CP
    for (int i = 0; i < counterCP; i++) {
        if (id == devicesCP[i].getID()) return i;
    }
    return INT_MIN;    //Se non trovo il device
}
int Interface::Mscan(int id) const noexcept {           //Indice e appartenenza di un device alla categoria M
    for (int i = 0; i < counterM; i++) {
        if (id == devicesM[i].getID()) return i;
    }
    return INT_MIN;   //Se non trovo il device
}
void Interface::updateKW() noexcept{        			//Aggiorna il numero di KW in uso
    KW = 0;												//Sommo tutte le variazioni, che con le routine
    std::vector<double> delta = timeline.getKWs(0, t);  //nascono problemi
    for (double d : delta) {
        KW += d;
    }
}
bool Interface::allowAutoTurnOff(int id) {          //Controllo se il dispositivo supporta l'autoTurnOff
    int pos = Mscan(id);
    return devicesM[pos].allowAutoTurnOff();
}

void timeCheck(int t, int start = INT_MIN, int end = INT_MIN) {     //Controllo della validità del tempo
    if (t < 0 || t > 1439) throw InvalidTimeException();				//tempo di input
	if (start != INT_MIN) {												//check di inizio routine
		if (start < 0 || start > 1439) throw InvalidTimeException();
		if (start < t) throw NotATimeMachineException();
	}
	if (end != INT_MIN) {											//check di termine routine
		if (end < 0 || end > 1439) throw InvalidTimeException();
		if (end < start) throw NotATimeMachineException();
	}
}
bool NSCcheck(std::string string1, std::string string2) noexcept {      //Comparazione case unsensitive
    if (string1.size() != string2.size()) return false;                 //Controllo preliminare
    std::transform(string1.begin(), string1.end(), string1.begin(), ::tolower);
    std::transform(string2.begin(), string2.end(), string2.begin(), ::tolower);

    return string1 == string2;  
}
std::string m2h(int m) noexcept {                                   //conversione da minuti a hh:mm
    //I casi in cui m va fuori dal range 0-1440 sono già coperti dalla timeline
    std::string hour = std::to_string(m / 60);    
    std::string minute = std::to_string(m % 60);            
    return (hour.size() == 1 ? '0' + hour : hour) + ":" + (minute.size() == 1 ? '0' + minute : minute);
}
void Lid(int len) {                     //hline della tabella
    for (int i = 0; i <= len; i++) {
        std::cout << "-";
    }
    std::cout << "\n";
}
void sortTimeBased(std::vector<int>& timestamps, std::vector<std::string>& events) {    // Riadattamento dell'insertion sort per ordinare gli eventi in base al tempo
    for (int i = 1; i < timestamps.size(); ++i) {
        int timestamp = timestamps[i];
        std::string event = events[i];
        int j = i - 1;

        while (j >= 0 && timestamps[j] > timestamp) {
            timestamps[j + 1] = timestamps[j];
            events[j + 1] = events[j];
            --j;
        }

        timestamps[j + 1] = timestamp;
        events[j + 1] = event;
    }
}
std::string _cleaner(std::string raw) {     //Rimuove gli underscore dai nomi in output
    std::string output = "";
    for (char c : raw) {
        if (c == '_') output+=" ";
        else output += c;
    }
    return output;
}
std::vector<int>& reverse(std::vector<int>& seq) {    //Inverte l'ordine di un vettore
    for (int i = 0; i < seq.size()/2; i++) {
        int temp = seq[i];
        seq[i] = seq[seq.size()-1-i];
        seq[seq.size()-1-i] = temp;
    }
    return seq;
}

//Funzioni membro
Interface::Interface(Logger *log, double KW, bool init, int maxDV, int time): maximumKW{KW}, maximumDV{maxDV}, t{time}   //Costruttore
{
    KW = 0;                         //KW utilizzati, all'inizio sono 0
    timeline.setRange(maximumDV);
    this -> log = log;              //Logger

    if (init) {             //Inizializzazione dei dispositivi secondo la consegna
        counterCP = 6;
        counterM = 4;

        devicesCP = {
            CPDevice(&timeline, &t, "Lavatrice", 0, 2, 110),
            CPDevice(&timeline, &t, "Lavastoviglie", 1, 1.5, 195),
            CPDevice(&timeline, &t, "Forno_microonde", 2, 0.8, 2),
            CPDevice(&timeline, &t, "Asciugatrice", 3, 0.5, 60),
            CPDevice(&timeline, &t, "Tapparelle", 7, 0.3, 1),
            CPDevice(&timeline, &t, "Televisore", 4, 0.2, 60)
        };
        devicesM = {
            ManualDevice(&timeline, &t, "Impianto_fotovoltaico", 5, -1.5),     //Pannello al contrario perché aumenta la soglia di KW disponibili
            ManualDevice(&timeline, &t, "Pompa_di_calore", 6, 2),
            ManualDevice(&timeline, &t, "Scaldabagno", 8, 1),
            ManualDevice(&timeline, &t, "Frigorifero", 9, 0.4, false)
        };
    }
}

void Interface::turnOn(int id) {                        //Accensione manuale
    updateKW();     //Aggiorno il numero di KW usati

    std::string msg;
	int Cpos = CPscan(id);                        //Controllo se l'ID è tra i dispositivi CP o M
	int Mpos = Mscan(id);                         //In caso positivo, salvo la posizione

    if (Cpos != INT_MIN) {
        if (devicesCP[Cpos].isOn())  throw DeviceAlreadyOnException();  //Se il dispositivo è già acceso lancio l'eccezione
        if (KW + devicesCP[Cpos].getConsumo() > maximumKW) {    //Se supero i KW lancio l'eccezione
            log ->log(Logger::ERROR, m2h(t), "OverKWException");
            throw OverKWException(); 
        }

        // Se c'è già una routine programmata nella fascia temporale del ciclo, cancello l'accensione
        std::vector<int> IDreq = timeline.getIDs(t, t + devicesCP[Cpos].getDurataCiclo());
        for (int i : IDreq) {
            if (i == id + maximumDV) {
                log ->log(Logger::ERROR, m2h(t), "TimerAlreadySetException");
                throw TimerAlreadySetException();
            }
        }

        //controllo per accensione futura
        std::vector<int> IDrequest = timeline.getIDs(t, t + devicesCP[Cpos].getDurataCiclo());
        for (int i : IDrequest) {
            if (i == id + maximumDV){
                log ->log(Logger::ERROR, m2h(t), "TimerAlreadySetException");
                throw TimerAlreadySetException();
            }
        }

        //Aggiornamento timeline (con anche lo spegnimento)
        msg = _cleaner(devicesCP[Cpos].getNome()) + " avviato";  //Messaggio di log
        std::cout << "[" << m2h(t) << "]: " << msg;                                                               //Output
        log ->log(Logger::EVENT, m2h(t), msg);                                         //Log
        //timeline
        timeline.addEvent(t, _cleaner(devicesCP[Cpos].getNome()) + " acceso", devicesCP[Cpos].getID() + maximumDV, devicesCP[Cpos].getConsumo(), false);
        timeline.addEvent(t + devicesCP[Cpos].getDurataCiclo(), _cleaner(devicesCP[Cpos].getNome()) + " spento", devicesCP[Cpos].getID(), -devicesCP[Cpos].getConsumo(), false);
    } else if (Mpos != INT_MIN) {
        if (devicesM[Mpos].isOn()) {
            log ->log(Logger::ERROR, m2h(t), "DeviceAlreadyOnException");
            throw DeviceAlreadyOnException();      //Se il dispositivo è già acceso lancio l'eccezione
        }
        if (KW + devicesM[Mpos].getConsumo() > maximumKW) {     //Se supero i KW lancio l'eccezione
            log ->log(Logger::ERROR, m2h(t), "OverKWException");
            throw OverKWException();
        }
        //Se c'è una routine futura, cancello l'accensione ma mantengo lo spegnimento
        std::vector<int> IDreq = timeline.getIDs(t);
        std::vector<int> timestamps = timeline.getTimes(t);
        for (int i = 0; i < IDreq.size(); i++) {
            if (IDreq[i] == id + maximumDV) {
                timeline.forget(id, t, timestamps[i]);
            }
        }
        msg = _cleaner(devicesM[Mpos].getNome()) + " acceso";
        std::cout << "[" << m2h(t) << "]: " << msg;
        log ->log(Logger::EVENT, m2h(t), msg);
        timeline.addEvent(t, _cleaner(devicesM[Mpos].getNome()) + " acceso", devicesM[Mpos].getID()+maximumDV, devicesM[Mpos].getConsumo(), false);
    } else {
        log ->log(Logger::ERROR, m2h(t), "ID non presente tra i dispositivi");
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}
void Interface::turnOn(int id, int start) {             //Accensione programmata
    timeCheck(t, start);			//Controllo e aggiorno
	updateKW();                 //Aggiorno il numero di KW usati

    //Ci possono essere più richieste di timer, quindi controllo se, in base alle richieste future
    //avrò abbastanza KW disponibili per far andare il dispositivo
    double KWonCall = 0;
    std::vector<double> programmedKW = timeline.getKWs(0, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestedKW : programmedKW) {
        KWonCall += requestedKW;                        //Sommo il tutto, dopo devo controllare di averne abbastanza liberi
    }

    std::string msg;
	int Cpos = CPscan(id);              //Controllo se l'ID è tra i dispositivi CP o M
	int Mpos = Mscan(id);               //In caso positivo, salvo la posizione

    if (Cpos != INT_MIN) {
        std::vector<int> IDreq = timeline.getIDs(t,start);
        for (int i = IDreq.size() - 1; i >= 0; i--) {   //Controllo che il device non sia già attivo al lancio
            if (IDreq[i] == id + maximumDV) {           //Se il dispositivo è già acceso lancio l'eccezione
                log ->log(Logger::ERROR, m2h(t), "TimerAlreadySetException");
                throw TimerAlreadySetException();
            }
            if (IDreq[i] == id) {                       //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }

		//Controllo che il device non presenti routine più brevi nella finestra temporale del ciclo
        IDreq = timeline.getIDs(start, start + devicesCP[Cpos].getDurataCiclo());
        for (int i = IDreq.size() - 1; i >= 0; i--) {                               //Controllo che il device non presenti accensioni nel momento della richiesta
            if (IDreq[i] == id + maximumDV) {
                log ->log(Logger::ERROR, m2h(t), "TimerAlreadySetException");
                throw TimerAlreadySetException();
            }
        }
        // Il dispositivo non sarà attivo nella finestra di tempo, ora controllo che non passi i KW
        if (KWonCall + devicesCP[Cpos].getConsumo() > maximumKW) {
            log ->log(Logger::ERROR, m2h(t), "OverKWException");
            throw OverKWException();
        }

        //Ora ho tutti i requisiti per impostare la programmazione
        msg = "Accensione di " + _cleaner(devicesCP[Cpos].getNome()) + " impostata";
        std::cout << "[" << m2h(t) << "]: " << msg;
        log ->log(Logger::EVENT, m2h(t), msg);
        timeline.addEvent(start, _cleaner(devicesCP[Cpos].getNome()) + " acceso", devicesCP[Cpos].getID() + maximumDV, devicesCP[Cpos].getConsumo(), true);
        timeline.addEvent(start+devicesCP[Cpos].getDurataCiclo(), _cleaner(devicesCP[Cpos].getNome()) + " spento", devicesCP[Cpos].getID(), -devicesCP[Cpos].getConsumo(), true);
    } else if (Mpos != INT_MIN) {
        std::vector<int> IDreq = timeline.getIDs(0,start);
        for (int i = IDreq.size() - 1; i >= 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (IDreq[i] == id + maximumDV) {
                log ->log(Logger::ERROR, m2h(t), "TimerAlreadySetException");
                throw TimerAlreadySetException();
            }
            if (IDreq[i] == id) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        //Controllo di avere abbastanza KW disponibili
        if (KWonCall + devicesM[Mpos].getConsumo() > maximumKW) {
            log ->log(Logger::ERROR, m2h(t), "OverKWException");
            throw OverKWException();
        }
        //Cancello eventuali programmazioni future
        timeline.forget(id, start);
        msg = "Accensione di " + _cleaner(devicesM[Mpos].getNome()) + " impostata";
        std::cout << "[" << m2h(t) << "]: " << msg;
        log ->log(Logger::EVENT, m2h(t), msg);
        timeline.addEvent(t, _cleaner(devicesM[Mpos].getNome()) + " acceso", devicesM[Mpos].getID() + maximumDV, devicesM[Mpos].getConsumo(), true);
    } else {
        log ->log(Logger::ERROR, m2h(t), "ID non presente tra i dispositivi");
        throw DeviceIDOutOfBoundException();
    }
}
void Interface::turnOn(int id, int start, int end) {    //routine
    timeCheck(t, start, end);
    updateKW();     //Aggiorno il numero di KW usati
    double KWonCall = 0;
    double MaxKWonCall = 0;
    std::vector<double> programmedKW = timeline.getKWs(0, start);   //Richiedo tutti i ΔKW da ora fino al momento dell'accensione
    for (double requestedKW : programmedKW) {
        KWonCall += requestedKW;                        //Sommo il tutto         
    }

    programmedKW = timeline.getKWs(start, end); //Controllo che nella routine non si superino mai i maximumKW
    double temp = KWonCall;
    for (double cycleKW : programmedKW) {
        temp += cycleKW;                        //Sommo il tutto  
        if (temp > MaxKWonCall) MaxKWonCall = temp;
    }

    std::string msg;
	int Cpos = CPscan(id);
	int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        log ->log(Logger::ERROR, m2h(t), "CPIllegalInstructionException");
        throw CPIllegalInstructionException();  //Se il dispositivo è a ciclo prefissato non si può accendere manualmente
    } else if (Mpos != INT_MIN) {
        std::vector<int> IDreq = timeline.getIDs(0, start);
        for (int i = IDreq.size() - 1; i >= 0; i--) {    //Controllo che il device non sia già attivo al lancio
            if (IDreq[i] == id + maximumDV) {
                log ->log(Logger::ERROR, m2h(t), "TimerAlreadySetException");
                throw TimerAlreadySetException();
            }
            if (IDreq[i] == id) {               //Se l'ultimo comando mandato era di spegnimento: OK
                break;
            }
        }
        IDreq = timeline.getIDs(start, end);  //Controllo che non inizino altri timer senza concludersi
        int count = 0;
        for (int i : IDreq) {
            if (i == id + maximumDV) count++;            //Conto che il numero di accensioni sia uguale a quello di spegnimenti
            if (i == id) count--;
        }
        if (count != 0) {
            log ->log(Logger::ERROR, m2h(t), "TimerAlreadySetException");
            throw TimerAlreadySetException();
        }
        timeline.forget(id, start, end);        //Cancello queste programmazioni più corte
        //Controllo che non si superino i KW
        if (KWonCall + devicesM[Mpos].getConsumo() > maximumKW) {
            log ->log(Logger::ERROR, m2h(t), "OverKWException");
            throw OverKWException();
        }
        if (MaxKWonCall + devicesM[Mpos].getConsumo() > maximumKW) {
            log ->log(Logger::ERROR, m2h(t), "OverKWException");
            throw OverKWException();
        }

        //Avvio la routine
        msg = "Routine di " + _cleaner(devicesM[Mpos].getNome()) + " impostata";
        std::cout << "[" << m2h(t) << "]: " << msg;
        log ->log(Logger::EVENT, m2h(t), msg);
        timeline.addEvent(start, _cleaner(devicesM[Mpos].getNome()) + " acceso", devicesM[Mpos].getID() + maximumDV, devicesM[Mpos].getConsumo(), true);
        timeline.addEvent(end, _cleaner(devicesM[Mpos].getNome()) + " spento", devicesM[Mpos].getID(), -devicesM[Mpos].getConsumo(), true);
    } else {
        log ->log(Logger::ERROR, m2h(t), "ID non presente tra i dispositivi");
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::turnOff(int id) {                       //spegnimento
    updateKW();     //Aggiorno il numero di KW usati

    std::string msg;
	int Cpos = CPscan(id);
	int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        log ->log(Logger::ERROR, m2h(t), "CPIllegalInstructionException");
        throw CPIllegalInstructionException();  //Se il dispositivo è a ciclo prefissato non si può spegnere manualmente
    } else if (Mpos != INT_MIN) {
        if (!devicesM[Mpos].isOn()) {
            log ->log(Logger::ERROR, m2h(t), "DeviceAlreadyOffException");
            throw DeviceAlreadyOffException();   //Se il dispositivo è già spento lancio l'eccezione
        }
        if (KW - devicesM[Mpos].getConsumo() > maximumKW) {
            log ->log(Logger::ERROR, m2h(t), "IllegalGeneratorShutdownException");
            throw IllegalGeneratorShutdownException();  //Se cerco di spegnere un generatore e i KW non bastano
        }
        timeline.forget(id, t+1);     //Cancello eventuali programmazioni future
        msg = _cleaner(devicesM[Mpos].getNome()) + " spento";
        std::cout << "[" << m2h(t) << "]: " << msg;
        log ->log(Logger::EVENT, m2h(t), msg);
        timeline.addEvent(t, _cleaner(devicesM[Mpos].getNome()) + " spento", devicesM[Mpos].getID(), -devicesM[Mpos].getConsumo(), false);
    } else {
        log ->log(Logger::ERROR, m2h(t), "ID non presente tra i dispositivi");
        throw DeviceIDOutOfBoundException();        //ID non presente tra i dispositivi.
    }
}
void Interface::forceOff(int id) {
    updateKW();
    std::vector<int> IDreq = timeline.getIDs(t);
    std::vector<int> timestamps = timeline.getTimes(t);

    int Cpos = CPscan(id);
    if (Cpos == INT_MIN) {      //Se non è un dispositivo CP
        log ->log(Logger::ERROR, m2h(t), "ForceOnMException");
        throw ForceOnMException();            //Passo il controllo al metodo di spegnimento normale
    }
    for (int i = 0; i < IDreq.size(); i++) {            //Ricerco quando doveva essere spento il device in futuro
        if (!devicesCP[Cpos].isOn()) {
            log ->log(Logger::ERROR, m2h(t), "DeviceAlreadyOffException");
            throw DeviceAlreadyOffException();
        }
        if (IDreq[i] == id) {
            timeline.forget(id, t+1, timestamps[i]);        // Aggiorno l'orario di spegnimento
            std::string msg = _cleaner(devicesCP[Cpos].getNome()) + " interrotto";
            std::cout << "[" << m2h(t) << "]: " << msg;
            log ->log(Logger::EVENT, m2h(t), msg);
            timeline.addEvent(t, _cleaner(devicesCP[Cpos].getNome()) + " interrotto", devicesCP[Cpos].getID(), -devicesCP[Cpos].getConsumo(), false);
        }
    }
}

void Interface::removeTimer(int id) {                   //Rimuove le routine di un device
	//Controllo che l'ID esista
    int Cpos = CPscan(id);
    int Mpos = Mscan(id);

	if ((Cpos == INT_MIN && Mpos == INT_MIN)) {
        log ->log(Logger::ERROR, m2h(t), "ID non presente tra i dispositivi");
        throw DeviceIDOutOfBoundException();
    }

    bool on;
    if (Cpos != INT_MIN) {
        on = devicesCP[Cpos].isOn();
    } else {
        on = devicesM[Mpos].isOn();
    }

    if (on) {
        timeline.forget(id, t);
        log ->log(Logger::INFO, m2h(t), "Timer di ID " + std::to_string(id) + " rimosso dall'ora " + std::to_string(t));
        return;
    }
    log ->log(Logger::INFO, m2h(t), "Timer di ID " + std::to_string(id) + " rimosso dall'ora " + std::to_string(t+1));
    timeline.forget(id, t+1);             //Elimino tutti gli eventi futuri legati all'elettrodomestico
}

void Interface::setTime(int time) {                     //Scorrimento del tempo
    timeCheck(t, time);                                                 // Controllo che il tempo sia valido

    std::vector<int> timestamps = timeline.getTimes(t,time);             // Vector con gli orari
    std::vector<std::string> events = timeline.getEvents(t,time);       // Vector con gli eventi
    sortTimeBased(timestamps, events);

    // Stampo la timeline
    std::cout << "[" << m2h(t) << "]: L'orario attuale e' " << m2h(t) << std::endl;
    
    for (size_t i = 0; i < timestamps.size(); ++i) {
        if (timestamps[i] != t) {
            log -> log(Logger::EVENT, m2h(timestamps[i]), events[i]);
        }
        std::cout << "[" << m2h(timestamps[i]) << "]: " << events[i] << std::endl;
    }
    
    std::cout << "[" << m2h(time) << "]: L'orario attuale e' " << m2h(time);
    
    t = time; // Aggiorno l'orario
    log -> log(Logger::INFO, m2h(t), "Orario aggiornato a " + std::to_string(time));
}

void Interface::resetTime() {                           //Resetta il tempo
    t = 0;                  //Ritorno con il tempo a 0
    timeline.removeNonRoutines();       //Pulisco la timeline
    std::cout << "[" << m2h(t) << "]: " << "Reset del tempo effettuato con successo" << std::endl;
    log -> log(Logger::INFO, m2h(t), "Orario resettato a 0");
}
void Interface::resetTimers() {                         //Resetta tutte le routine
    for (int i = 0; i < counterCP; i++) {
        timeline.forget(devicesCP[i].getID(), t);
    }
    for (int i = 0; i < counterM; i++) {
        timeline.forget(devicesM[i].getID(), t);
    }
    std::cout << "[" << m2h(t) << "]: " << "Reset dei timer effettuato con successo" << std::endl;
    log -> log(Logger::INFO, m2h(t), "Timer resettati");
}
void Interface::resetAll() {                         //Resetta tutte le routine
    t = 0;
    timeline.clear();
    std::cout << "[" << m2h(t) << "]: " << "Reset effettuato con successo" << std::endl;
    log -> log(Logger::INFO, m2h(t), "Reset del sistema effettuato");
}

void Interface::show() {                                //Mostra tutti i device
    std::cout << "[" << m2h(t) << "]: Stato dei devices:" << std::endl;
    std::vector<std::string> production;

    int cols = 4;
    int nameCol = 21;
    int statusCol = 6;
    int usageCol = 5;       //Problema
    int runningCol = 4;


    int sum = nameCol + statusCol + usageCol + runningCol + 3 * cols + 1;    //Numero di spazi totali
    Lid(sum);
    //Struttura | COL | COL | COL | COL |
    for (int i = -1; i < counterM; i++) {
        if (i < 0) {
            int diff;
            std::string headers[5] = {"Nome Device", "Stato ", "KW", "Ore  "};
            std::string emptyspaces1;
            std::string emptyspaces2;

            diff = nameCol - headers[0].size();
            for (int i = 0; i < diff; i++) {
                emptyspaces1 += " ";
            }
            diff = usageCol - headers[2].size();
            for (int i = 0; i < diff; i++) {
                emptyspaces2 += " ";
            }
            std::cout << "| " << headers[0] << emptyspaces1 << " | " << headers[1] << " | " << headers[2] << emptyspaces2 << " | " << headers[3] << " |" << std::endl;
            Lid(sum);
        } else {     
            if (devicesM[i].getConsumo() < 0) {
                double temp = -devicesM[i].getConsumoTotale();
                production.push_back(_cleaner(devicesM[i].getNome()) + " ha prodotto " + std::to_string(temp) + "KW in " + m2h(devicesM[i].getTempoDiEsecuzione()) + " ore");
                continue;
            }   
            int diff;
            std::string emptyspaces1;

            diff = nameCol - devicesM[i].getNome().size();
            for (int i = 0; i < diff; i++) {
                emptyspaces1 += " ";
            }
            std::cout << "| " << _cleaner(devicesM[i].getNome()) << emptyspaces1 << " | " << (devicesM[i].isOn()? "acceso" : "spento") << " | " << std::fixed << std::setprecision(3) << devicesM[i].getConsumoTotale() << " | " << m2h(devicesM[i].getTempoDiEsecuzione()) << " |" << std::endl;
        }
    }
    for (int i = 0; i < counterCP; i++) {
        if (devicesCP[i].getConsumo() < 0) {
            double temp = -devicesCP[i].getConsumoTotale();
            production.push_back(_cleaner(devicesCP[i].getNome()) + " ha prodotto " + std::to_string(temp) + "KW in " + m2h(devicesCP[i].getTempoDiEsecuzione()) + " ore");
            continue;
        }
        int diff;
        std::string emptyspaces1;

        diff = nameCol - devicesCP[i].getNome().size();
        for (int i = 0; i < diff; i++) {
            emptyspaces1 += " ";
        }
        std::cout << "| " << _cleaner(devicesCP[i].getNome()) << emptyspaces1 << " | " << (devicesCP[i].isOn()? "acceso" : "spento") << " | " << std::fixed << std::setprecision(3) << devicesCP[i].getConsumoTotale() << " | " << m2h(devicesCP[i].getTempoDiEsecuzione()) << " |" << std::endl;
    }
    Lid(sum);
    for (int i = 0; i < production.size(); i++) {
        std::cout << production[i] << std::endl;
    }
    log -> log(Logger::INFO, m2h(t), "Stato dei devices mostrato");
}
void Interface::show(int id) {                          //Mostra un device
    int Cpos = CPscan(id);
    int Mpos = Mscan(id);

    if (Cpos != INT_MIN) {
        std::cout   << _cleaner(devicesCP[Cpos].getNome())
					<< "[" 
					<< (devicesCP[Cpos].isOn()? "acceso" : "spento") 
					<< "]" 
					<< (devicesCP[Cpos].getConsumo() < 0? " ha prodotto " : " ha consumato ") 
					<< (devicesCP[Cpos].getConsumo() < 0? -devicesCP[Cpos].getConsumo() : devicesCP[Cpos].getConsumo())
					<< "KW, oggi e' stato usato per " 
					<< m2h(devicesCP[Cpos].getTempoDiEsecuzione())
					<< " ore\n";
        log -> log(Logger::INFO, m2h(t), "Device " + std::to_string(id) + " mostrato");
    } else if (Mpos != INT_MIN) {
        std::cout   << _cleaner(devicesM[Mpos].getNome())
					<< "[" 
					<< (devicesM[Mpos].isOn()? "acceso" : "spento") 
					<< "]" 
					<< (devicesM[Mpos].getConsumo() < 0? " ha prodotto " : " ha consumato ")
					<< (devicesM[Mpos].getConsumo() < 0? -devicesM[Mpos].getConsumo() : devicesM[Mpos].getConsumo() )
					<< "KW, oggi e' stato usato per " 
					<< m2h(devicesM[Mpos].getTempoDiEsecuzione())
					<< " ore \n";
        log -> log(Logger::INFO, m2h(t), "Device " + std::to_string(id) + " mostrato");
    } else {
        log -> log(Logger::ERROR, m2h(t), "ID non presente tra i dispositivi");
        throw DeviceIDOutOfBoundException();
    }
}

void Interface::installM(std::string name, double consumo, bool autoTurnOff, bool isOn) {     //Installa device M
    //Controllo che non sia già presente
    for (int i = 0; i < counterM; i++) {
        if (NSCcheck(devicesM[i].getNome(), name)) {
            log -> log(Logger::ERROR, m2h(t), "DuplicateDeviceException");
            throw DuplicateDeviceException();
        }
    }
	//Creazione ID
    int id;
    if (counterM + counterCP >= maximumDV) {
        log -> log(Logger::ERROR, m2h(t), "DeviceLimitException");
        throw DeviceLimitException(); // Eccezione, troppi Device installati
    }
    counterM++;
    if (freeID.size() > 0) {			//Se ho degli id liberi, li uso
        id = freeID[0];
        freeID.erase(freeID.begin());
    } else {
        id = counterM+counterCP;		//Altrimenti, ne creo uno nuovo
    }
    devicesM.push_back(ManualDevice(&timeline, &t, name, id, consumo, autoTurnOff, isOn));
    if (consumo < 0) {
        std::cout << "[" << m2h(t) << "]: " << "Installato " << _cleaner(name) << "[" << id << "] con produzione " << -consumo << "KW";
    } else {
        std::cout << "[" << m2h(t) << "]: " << "Installato " << _cleaner(name) << "[" << id << "] con consumo " << consumo << "KW";
    }
    log -> log(Logger::INFO, m2h(t), "Device " + name + " installato, ID: " + std::to_string(id) + ", consumo: " + std::to_string(consumo) + "KW, autoTurnOff: " + std::to_string(autoTurnOff) + ", isOn: " + std::to_string(isOn));
}
void Interface::installCP(std::string name, double consumo, int durataCiclo, bool isOn) {   //Installa device CP
    //Check che non sia già presente
    for (int i = 0; i < counterCP; i++) {
        if (NSCcheck(devicesCP[i].getNome(), name)) {
            log -> log(Logger::ERROR, m2h(t), "DuplicateDeviceException");
            throw DuplicateDeviceException();
        }
    }
	//Creazione ID
    int id;
    if (counterM + counterCP >= maximumDV) {
        log -> log(Logger::ERROR, m2h(t), "DeviceLimitException");
        throw DeviceLimitException(); // Eccezione, troppi Device installati
    }
    counterCP++;
    if (freeID.size() > 0) {		//Se ho degli ID liberi, li uso
        id = freeID[0];
        freeID.erase(freeID.begin());
    } else {
        id = counterM+counterCP;	//Altrimenti, ne creo uno nuovo
    }
    devicesCP.push_back(CPDevice(&timeline, &t, name, id, consumo, durataCiclo, isOn));
    if (consumo < 0) {
        std::cout << "[" << m2h(t) << "]: " << "Installato " << _cleaner(name) << "[" << id << "] con ciclo " << m2h(durataCiclo) << "h e produzione " << -consumo << "KW";
    } else {
        std::cout << "[" << m2h(t) << "]: " << "Installato " << _cleaner(name) << "[" << id << "] con ciclo " << m2h(durataCiclo) << "h e consumo " << consumo << "KW";
    }
    log -> log(Logger::INFO, m2h(t), "Device " + name + " installato, ID: " + std::to_string(id) + ", consumo: " + std::to_string(consumo) + "KW, durataCiclo: " + std::to_string(durataCiclo) + "h, isOn: " + std::to_string(isOn));
}
void Interface::uninstall(int id) {                     //Disinstalla un device
    std::string name;
    for (int i = 0; i < counterCP; i++) {			//Controllo se il device è CP
        if (devicesCP[i].getID() == id) {
            int pos = CPscan(id);
            if (devicesCP[pos].isOn()) {
                log -> log(Logger::ERROR, m2h(t), "DeviceIsRunningException");
                throw DeviceIsRunningException();
            }
            name = devicesCP[i].getNome();
            devicesCP.erase(devicesCP.begin() + pos);
            freeID.push_back(id);						//Segno il suo ID come libero
			counterCP--;
            log -> log(Logger::INFO, m2h(t), "Device " + name + " disinstallato, ID: " + std::to_string(id));
            std::cout << "[" << m2h(t) << "]: " << "Disintallato il device " << name << "[" << id << "]";
            return;
        }
    }
    for (int i = 0; i < counterM; i++) {			//Controllo tra i device M
        if (devicesM[i].getID() == id) {
            int pos = Mscan(id);
            if (devicesM[pos].isOn()) {
                log -> log(Logger::ERROR, m2h(t), "DeviceIsRunningException");
                throw DeviceIsRunningException();
            }
            name = devicesM[i].getNome();
            devicesM.erase(devicesM.begin() + pos);
            freeID.push_back(id);					//Segno l'ID come libero
			counterM--;	
            log -> log(Logger::INFO, m2h(t), "Device " + name + " disinstallato, ID: " + std::to_string(id));
            std::cout << "[" << m2h(t) << "]: " << "Disintallato il device " << name << "[" << id << "]";
            return;
        }
    }
    log -> log(Logger::ERROR, m2h(t), "ID non presente tra i dispositivi");
    throw DeviceIDOutOfBoundException();	//Il device non è presente tra i devices
}

int Interface::searchID(std::string name) {			//Ritorna l'ID dato il nome
    log -> log(Logger::INFO, m2h(t), "Ricerca ID per nome" + name);
	for (int i = 0; i < counterCP; i++) {
		if (NSCcheck(devicesCP[i].getNome(), name)) return devicesCP[i].getID();	//Controllo i CP
	}
	for (int i = 0; i < counterM; i++) {
		if (NSCcheck(devicesM[i].getNome(), name)) return devicesM[i].getID();	//Controllo gli M
	}
    log -> log(Logger::ERROR, m2h(t), "NameNotFoundException");
	throw NameNotFoundException();
}
std::vector<int> Interface::turnOffSequence() {
    std::vector<int> sequence;
    std::vector<int> OnDevices;
    std::vector<int> IDreq = timeline.getIDs(0, t);             //Ordine di accensione

    for (auto device : devicesM) {                              // Controllo quali dispositivi possono essere spenti
        if (device.allowAutoTurnOff() && device.isOn() && device.getConsumo() > 0) {
            OnDevices.push_back(device.getID());
        }
    }
    for (int i = IDreq.size() - 1; i >= 0; i--) {       //Controllo l'ordine di accensione, dal più recente al più vecchio
        for (int j : OnDevices) {
            if (IDreq[i] == j + maximumDV) {
                sequence.push_back(j);
                break;
            }
        }
        if (sequence.size() == OnDevices.size()) break;     //Se ho trovato la sequenza, esco
    }
    return reverse(sequence);                           //Inverto la sequenza, così restituisco dal meno recente
}

double Interface::debugKWs() {            //Debug KW
    updateKW();
    std::cout << "#DK" << KW << std::endl;
    log -> log(Logger::DEBUG, m2h(t), "KW attuali: " + std::to_string(KW));
    return KW;
}
int Interface::debugTime(bool logging) {              //Debug time
    if (!logging) {
        log -> log(Logger::DEBUG, m2h(t), "Orario attuale: " + std::to_string(t));
        std::cout << "#DT " << t << std::endl;
    }
    return t;
}
void Interface::debugCounters() {         //Debug counters
    log -> log(Logger::DEBUG, m2h(t), "Contatori: CP: " + std::to_string(counterCP) + " M: " + std::to_string(counterM));
    std::cout << "#DC CP: " << counterCP << " M:" << counterM << std::endl;
}
void Interface::debugTOS() {
    log -> log(Logger::DEBUG, m2h(t), "Calcolo TOS");
    std::cout << "CALCOLO TOS" << std::endl;
    std::vector<int> sequence = turnOffSequence();
    std::cout << std::endl;
    if (sequence.size() == 0) {
        std::cout << "[]" << std::endl;
        return;
    }
    log -> log(Logger::DEBUG, m2h(t), "TOS: ");
    std::cout << "#TOS [";
    for (int i : sequence) {
        log -> log(Logger::DEBUG, m2h(t), std::to_string(i));
        std::cout << i << ", ";
    }
    std::cout << "\b\b]" << std::endl;
    log -> log(Logger::DEBUG, m2h(t), "SIZE = " + std::to_string(sequence.size()));
    std::cout << "SIZE = " << sequence.size() << std::endl;
}