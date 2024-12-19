# Sistema domotico

ᕙ(⇀‸↼‶)ᕗ

*Andreose merda durante la foto di gruppo*

**P.S. Non consegnare questo README porcodio!**

## Suddivisione progetto

**Riccardo**: Device.h e le classi derivate.

**Andrea**: Interface.h e tutta quella roba lì.

**Giacomo**: main.cpp, parte di test e robe varie.

**Riccavdo Andveose e Massimo Sturato**: Bocchini sottobanco e botte.

## Passaggio di dati tra moduli
* Il tempo lo passiamo per **minuti**, quindi `set time 8:30` chiamatelo con `setTime(510)`, che è più comodo per fare il delta del tempo trascorso. Stessa roba per il cronometro di `Driver`, durata del ciclo e blablabla.

* Per accendere/spegnere i dispositivi usiamo l'**ID**, per esempio per spegnere la pompa si va `turnOff(4)`.

* Gli **ID** li ho messi a partire da 1, un po' perché sono stronzo, un po' perché è più bello sul frontend (forse). L'ordine degli ID è quello del file, prima i device CP e poi quelli manuli, in caso:

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

* Nella cartella exceptions metteremo (indovinate) le classi di errore. Poi boh le importiamo e via