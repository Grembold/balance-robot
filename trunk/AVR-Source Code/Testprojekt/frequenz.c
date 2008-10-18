volatile uint8_t countTimer2;	// Speichert den aktuellen Zählerwert

// ISR zum auffangen der Interrupts:
SIGNAL(SIG_OUTPUT_COMPARE2)
{
  countTimer2++;
}

// Initialisierung:
TCCR2 = (1<<CS22) | (1<<WGM21);	// Prescaler von 1 | CTC-Modus (siehe unten für Beschreibung)
OCR2  = 73;			// Vergleichswert
TIMSK |= (1<<OCIE2);		// Interrupts aktivieren und damit Timer starten
sei();

// Funktionen zum benutzen der Timer:
/** Diese Funktion nicht aufrufen. Wird von sleep_millisec aufgerufen.
Bei t=100 schläft die Funktion 1 ms. */
inline void sleep(uint8_t t)
{
	// countTimer2 wird in der ISR oben inkrementiert
	countTimer2 = 0;
	while (countTimer2 < t);
}

/** Schläft x-Millisekunden. */
inline void sleep_millisec(uint16_t msec)
{
	uint16_t i;
	for(i=0; i<msec; i++) {
		sleep(100);
	}
}
