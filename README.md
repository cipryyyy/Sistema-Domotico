# Progetto finale - sistema domotico

## Suddivisione progetto

**Buso Riccardo**: `Device.h` e le classi derivate `ManualDevice.h`e `CPDevice.h`

**Cipriani Andrea**: `Interface.h` e `Timeline.h`

**Giorgi Giacomo**: `main.cpp`, parte di test, registrazione dei log

## Passaggio di dati tra moduli
* Il tempo lo passiamo per **minuti**, quindi `set time 8:30` chiamatelo con `setTime(510)`, che è più comodo per fare il delta del tempo trascorso. Stessa roba per il cronometro di `Driver`, durata del ciclo e blablabla.

* Per accendere/spegnere i dispositivi usiamo l'**ID**, per esempio per spegnere la pompa si va `turnOff(4)`.

| **Elettrodomestico**        | **ID** |
|-----------------------------|--------|
| Lavatrice                   | 1      |
| Lavastoviglie               | 2      |
| Forno a microonde           | 3      |
| Asciugatrice                | 4      |
| Televisore                  | 5      |
| Fotovoltaico                | 6      |
| Pompa di calore             | 7      |
| Tapparelle                  | 8      |
| Scaldabagno                 | 9      |
| Frigorifero                 | 10     |

* Gli ID si possono recuperare dal nome

* Nella cartella exceptions metteremo (indovinate) le classi di errore

* La classe timeline.h simula una linea temporale.