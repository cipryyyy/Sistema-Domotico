Note per la consegna

# main.cpp:

# Interface.h / Interface.cpp

Questa classe si occupa di gestire tutti i diversi device, è articolata in 3 file:
- Interface.h           HEADER con le firme
- Interface.cpp         Definizioni dei metodi
- InterfaceExcpetions.h Libreria con gli errori

# Timeline.h

Ho creato questa libreria per gestire lo scorrimento del tempo, essenzialmente va a creare un oggetto composto da 4 vettori:
- Orario dell'evento
- Descrizione dell'evento
- ID del device che deve compiere l'evento
- Variazione di KW dell'impianto

I 4 vettori sono collegati dall'indice, quindi l'evento descritto in posizione 4 ha il tempo in posizione 4, l'ID in 4 e la variazione in 4.
Gli array rimangono disordinati, principalmente perché l'inserimento dati ha una complessità temporale di O(1), mentre la richiesta di dati ha una complessita di O(n)
Siccome le richieste di inserimento di dati sono superiori a quelle di lettura ho preferito mantanere l'ordine di inserimento dei dati.

Per sapere se un device è acceso o spento si potrebbe collegare un array di bool all'orario di un evento, ma se, ipoteticamente, avessi un evento ogni minuto, avrei 1440 array in memoria.
Per evitare questo spreco il vettore degli ID non solo salva l'ID del device, ma salva anche se viene acceso o spento in un dato momento.
Per fare ciò se il device viene acceso il suo ID è incrementato di 10, altrimenti rimane normale. Questo perché gli ID maggiori di 10 non esistono.