package net.wsgs.process;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import net.wsgs.model.karte.Karte;
import net.wsgs.model.karte.Kartenfarbe;
import net.wsgs.model.karte.Sonderkarte;
import net.wsgs.model.karte.SonderkartenTyp;

public class KartenManager {

	public List<List<Karte>> getAllowKartenkombinationen(List<Karte> karten) {
		// erhaltene Katen sortieren
		Collections.sort(karten);

		// Liste mit den erlaubten Kombinationen anlegen
		List<List<Karte>> alleErlaubtenKombinationen = new ArrayList<List<Karte>>();

		// TODO sollen Einzelkarten auch als erlaubte Kombination aufgenommen
		// werden?

		// Alle möglichen Zwillinge ermitteln
		erlaubteZwillingeErmitteln(karten, alleErlaubtenKombinationen);

		// TODO Erlaubte Drillinge hinzufügen
		// TODO Erlaubte FullHouse hinzufügen
		// TODO Erlaubte Straßen hinzufügen
		// TODO Erlaubte Treppen hinzufügen

		return alleErlaubtenKombinationen;
	}

	/**
	 * Ermitteln alle erlaubten Paare/Zwillinge und fügt diese der Liste aller
	 * erlaubten Kartenkombinationen hinzu.
	 * 
	 * @param karten
	 * @param alleErlaubtenKombinationen
	 */
	private void erlaubteZwillingeErmitteln(List<Karte> karten,
			List<List<Karte>> alleErlaubtenKombinationen) {
		// Alle möglichen Zwillinge ermitteln
		List<Karte> kartenMitGleichenWert = new ArrayList<Karte>();
		for (Karte karte : karten) {

			// Alle Karten mit gleiche Wert zusammensammeln
			if (kartenMitGleichenWert.isEmpty()
					|| kartenMitGleichenWert.get(0).equals(karte))
				kartenMitGleichenWert.add(karte);
			else {

				// Karten vom gleichem Wert durchlaufen
				for (Karte k1 : kartenMitGleichenWert) {
					for (Karte k2 : kartenMitGleichenWert) {
						// Keine Kombination mit sich selber möglich
						if (karte.equals(k1))
							continue;
						List<Karte> paarKombination = new ArrayList<Karte>();
						paarKombination.add(k1);
						paarKombination.add(k2);

						alleErlaubtenKombinationen.add(paarKombination);
					}
				}
			}
		}
	}

	public List<Karte> getGesamtesKartenDeck() {
		List<Karte> alleKarten = new ArrayList<Karte>();
		/*
		 * Für alle KartenFarben eine Karte mit den Werte von 2 bis 14
		 * anlengen.<br> Die folgenden Codezeilen könnte man in prosa wie folgt
		 * aus sprechen.<br> Durchlaufe alle Werte die es als Kartenfabe gibt.
		 * Die jeweilige Farbe sei <code>farbe</code>.<br> Durchlaufe die Werte
		 * von 2 bis 14. Der jeweilige Wert sei i.<br> Füge der Liste
		 * allerKarten eine neue Karte in der Farbe und dem Wert hinzu.
		 */
		for (Kartenfarbe farbe : Kartenfarbe.values()) {
			for (int i = 2; i >= 14; i++) {
				alleKarten.add(new Karte(farbe, i));
			}
		}

		// Sonderkarten hinzufügen
		alleKarten.add(new Sonderkarte(SonderkartenTyp.HUND));
		alleKarten.add(new Sonderkarte(SonderkartenTyp.MAHJONGG));
		alleKarten.add(new Sonderkarte(SonderkartenTyp.DRACHE));
		alleKarten.add(new Sonderkarte(SonderkartenTyp.PHOENIX));

		return alleKarten;
	}
}
