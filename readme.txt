Note per la consegna

SUDDIVISIONE DEL PROGETTO:
* Buso Riccardo: Progettazione dei moduli Device, ManualDevice, CPDevice e del CMakeLists.txt
* Cipriani Andrea: Progettazione dei moduli Interface e Timeline
* Giorgi Giacomo: Progettazione di main e Logger

L'intero progetto è stato fatto con l'utilizzo di github per il controllo versione.

# main.cpp:
    Il file main.cpp rappresenta il punto di ingresso del sistema e implementa l'interfaccia utente del programma di gestione energetica. La sua architettura è organizzata attorno a diverse funzioni helper chiave che facilitano l'interazione con l'utente e la gestione dei comandi:
        La gestione dell'input utente è gestita attraverso funzioni specializzate:

        - tokenize(): scompone i comandi inseriti in token separati, gestendo correttamente gli spazi e permettendo un'analisi precisa dei comandi
        - isValidTimeFormat(): verifica che il formato del tempo inserito rispetti gli standard HH:MM o H:MM
        - parseTime(): converte le stringhe temporali nel formato interno del sistema (minuti dalla mezzanotte)
        - displayHelp(): fornisce documentazione dettagliata dei comandi, sia generale che specifica per comando

        Il sistema offre due modalità di configurazione della potenza massima, dispositivi massimi:
        1. Tramite argomento da riga di comando all'avvio del programma
            - Il una volta compilato, si può eseguire con il seguente prompt: ./SmartHome <Max KW> <Max Dispositivi> <debug>
                - dove i tre argomenti sono facoltativi. Si possono inserire le seguenti combinazioni:
                    - ./SmartHome
                    - ./SmartHome <Max KW> 
                    - ./SmartHome <Max KW> <Max Dispositivi>
        2. Tramite input utente al primo avvio del sistema

        Da notare, per attivare la modalità di debug, basta appendere alla fine del prompt di avvio la parola chiave "debug".

        La struttura del programma include:
        - Costanti di sistema configurabili (MAX_POWER, MAX_DEVICES)
        - Sistema di logging per tracciare le operazioni
        - Gestione completa delle eccezioni per garantire la robustezza del sistema
        - Modalità debug attivabile per il troubleshooting

    Il main loop del programma processa continuamente i comandi dell'utente, interpretandoli e instradandoli alle appropriate funzioni di gestione, mantenendo un sistema di logging attivo per tracciare tutte le operazioni eseguite.

# Logger.h:
    Il file Logger.h implementa un sistema di logging robusto e flessibile per tracciare le operazioni del sistema di gestione energetica.
    La classe Logger è progettata con particolare attenzione alla gestione delle risorse e alla sicurezza:
    - Utilizza il RAII (Resource Acquisition Is Initialization) per la gestione del file di log
    - Implementa la Rule of Five: disabilita la copia (costruttore e operatore di assegnazione), mentre permette il movimento degli oggetti
    - Gestisce correttamente le eccezioni durante l'apertura dei file

    Caratteristiche principali:
    - Supporta diversi livelli di log (DEBUG, USERINPUT, EVENT, ERROR, INFO)
    - Genera automaticamente nomi file basati sulla data corrente
    - Include timestamp nei messaggi di log
    - Supporta una modalità debug attivabile/disattivabile
    - Gestisce automaticamente l'apertura e chiusura dei file

    Funzionalità specifiche:
    - getTimestamp(): genera timestamp nel formato YYYYMMDD per i nomi dei file
    - log(): metodo principale per la registrazione dei messaggi, con supporto per diversi livelli di dettaglio
    - Flushing automatico dopo ogni operazione di scrittura per garantire la persistenza dei dati
    - Gestione efficiente delle risorse con chiusura automatica dei file nel distruttore
    Il sistema è progettato per essere thread-safe e per gestire correttamente le risorse di sistema, con particolare attenzione alla gestione della memoria e dei file.
    Ho deciso di far generare un singolo file di log al giorno e non un file per ogni avvio per una questione di praticità. Nell'eventualità che questo programma venga avviato
    nel mondo reale, è molto più pratico avere un singolo file per quel giorno, rendendo più facile capire dove si trova un eventuale problema. Qualora si volesse rendere più
    granulare il sistema di logging, basta aggiungere alla generazione di getTimestamp gli shorthand per ora/minuti/secondi alla stringa e di conseguenza si genereranno più file,
    in base a quante volte viene spento e riacceso il programma durante la giornata.

# Interface.h / Interface.cpp
    Il modulo Interface si occupa di gestire la comunicazione tra l'input utente e i device, facendo lavorare in sicurezza l'intero sistema, si occupa di gestire
    gli oggetti di Timeline.h e Device.h.
    Interface si articola nell'header (include/Interface.h), nel file con le definizioni (src/Interface.cpp) e nel file con le eccezioni (exceptions/InterfaceExceptions.h).
    I metodi principali sono:
        - turnOn,                              accensione classica e programmata dei device
        - turnOff,                             spegnimento dei device
        - forceOff,                            interrompe l'esecuzione di un device a ciclo programmato
        - setTime,                             fa scorrere il tempo
        - resetTime, resetTimers, resetAll,    resettano i vari parametri come tempo, timers o il programma in generale
        - show,                                mostra lo stato di uno o tutti i dispositivi
        - installM, installCP                  installano device manuali e a ciclo programmato
        - uninstall,                           Disinstalla un dispositivo

    Oltre a queste funzioni sono presenti una serie di funzioni helper che servono di supporto, come il TimeCheck, Mscan/CPscan, searchID...
    Una su cui vorrei fare un appunto è updateKW(), dato che ci sono stati diversi problemi nel conteggio dei KW utilizzano multiple routine,
    ho optato per fare una funzione che ogni volta che si chiama una funzione di modifica dei KW (accensione e spegnimento dei device) aggiorno
    il numero di KW, così posso fare i calcoli e valutare se posso accendere il device / se ne devo spegnere altri.

    Per il resto il programma fa un uso pesante del passaggio di dati alle funzioni per copia, fatta eccezione per il passaggio dei vettori, per quelli
    ho preferito fare un passaggio per reference in quanto lo ritengo più adatto.

    Le eccezioni sono tutte commentate all'interno del file in cui sono contenute.

    Per quanto riguarda le funzioni di debug, queste vengono chiamate da main solo se è in modalità DEBUG e forniscono informazioni circa il numero di elementi, valori della classe e passaggi svolti durante l'esecuzione dalla classe.

    L'unico 'spreco' di memoria (per quanto leggero), sono counterCP e counterM, sono banalmente equivalenti a Device.size(), ma 
    tuttora non riesco a far funzionare il programma se li sostituisco.

# Timeline.h
    Timeline è una libreria che genera oggetti capaci di tener traccia degli eventi del sistema.
    Questo modulo nasce per riuscire a sostenere più routine su un singolo device, per esempio se volessi accendere il termosifone in 4 intervalli della giornata con timeline posso fare tutto subito
    senza attendere che la routine precedente finisca. Inoltre è molto utile per tenere traccia delle varie accensioni, spegnimenti, routine future e passate

    La Timeline è composta da 5 oggetti vector contenenti:
        - I timestamp dell'evento
        - Una breve descrizione testuale dell'evento (Per rendere chiaro all'utente cos'è successo con la funzione setTime di Interface)
        - L'ID dell'evento(*)
        - La variazione di KW (usata da UpdateKW)
        - la natura dell'accensione/spegnimento (routine o comando manuale) (**)

    (*) Per tenere traccia delle accensioni e degli spegnimenti ho voluto strutturare il vector ID in maniera particolare.
    Premetto che il sistema può gestire un massimo di Device salvato in maximumDV (Interface) e passato in range (Timeline)

    È impossibile che esista nel sistema un device con ID == range, perciò ho voluto sfruttare questa cosa.
    Se un Device viene acceso, in ID salverò il suo ID + range, mentre se viene spento salvo solo ID, in questa maniera non solo so chi ha compiuto l'azione, ma so pure se si è acceso e spento,
    senza fare un parsing del vettore eventi (che richiederebbe più passaggi, o comunque sarebbe meno elegante a parer mio con altre maniere).

    Come eccezioni Timeline presenta dei banali invalid_argument, questo perché li ho messi più per una doppia sicurezza, nel caso qualcun altro dovesse usare il modulo,
    infatti le eccezioni sono facilmente evitate già da Interface, è una ripetizione messa li come sicurezza per un utente diverso che non conosce bene il meccanismo di timeline.

    (**) Quando faccio un resetTime io ritorno al tempo 0, il problema è che io nella timeline ho salvato tutte le azioni, anche quelle manuali.
    Per conservare i timer e cancellare le azioni manuali devo essere in grado di identificare la natura dell'accensione/spegnimento, questo vector serve a questo.

    Le funzioni più importanti sono:
        - addEvent,                        Aggiunge un evento alla timeline
        - getTimes, Events, Ids, KWs,      Ritornano delle copie dei vettori con i valori contenuti in un certo lasso di tempo
        - forget,                          Elimina le azioni di un ID in un lasso di tempo
        - clear,                           Elimina tutto dalla timeline
        - setRange,                        Imposta il valore di range, siccome una sua modifica potrebbe invalidare l'intero programma, viene chiamata solo nella costruttore di interface
        - getRange,                        Ritorna il valore di range, utile per i device per i comandi isOn, getTempoDiEsecuzione..
        - removeNonRoutines,               esegue quanto detto in (**)

# Device.h / Device.cpp
    Questa libreria serve a rappresentare dei device elettrici di uso domestico/industriale.
    Classe padre di ManualDevice e CPDevice, racchiude tutte le caratteristiche che i device hanno in comune.
    Utilizza la libreria Timeline per gestire lo scorrere del tempo.
    La libreria Device.h rende disponibili le seguenti funzioni:
        -   isOn()                      Ritorna True/False, se il device è acceso o spento 
        -   getNome()                   Ritorna il nome del device
        -   getID()                     Ritorna l'ID del device 
        -   getConsumo()                Ritorna il consumo del device 
        -   getTempoDiEsecuzione()      Ritorna quanto tempo è stato acceso il device durante la giornata, in minuti 
        -   getConsumoTotale()          Ritorna quanto il device ha consumato durante la giornata 
        -   setTempoDiEsecuzione()      Serve per modificare la durata di esecuzione dei device 

        Gli altri attributi vengono impostati solo dal costruttore per scelta progettuale.

        Sia la funzione isOn che getTempoDiEsecuzione sfruttano la libreria Timeline per capire quando e quanto il device è stato acceso,
        e calcolare la propria richiesta.
        Queste due funzioni potevano essere sviluppate in maniera differente, rendendole meno dipendenti da Timeline e aumentando così l'incapsulamento,
        ma abbiamo scelto questo approccio per questione di tempo e di praticità del codice.

# ManualDevice.h / ManualDevice.cpp

    Questa classe estende la classe Device per rappresentare al meglio i device manuali.
    Implementa un attributo (autoTurnOff) che serve a indicare se il device supporta la politica di spegnimento automatico.
     
    Es: nel caso di overKW, alcuni device possono essere spenti mentre altri non possono.
    a scelta del utilizzatore.

# CPDevice.h / CPDevice.cpp

    Questa classe estende la classe Device e serve a rappresentare i device a ciclo preimpostato.
    Include un attributo durataCiclo, che indica quanto dura l'esecuzione di un device CP.
    Allo scadere del tempo il device si spegne automaticamente.
    (Questo è gestito dall'esterno della classe).

    Questa classe può lanciare un'eccezione se il valore inserito come durata del ciclo non è valido.
    Eccezione = ValoreNonValido().
