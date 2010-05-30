package net.wsgs.process;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import net.wsgs.model.karte.Karte;

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
					&& kartenMitGleichenWert.get(0).equals(karte))
				kartenMitGleichenWert.add(karte);
			else {

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
}
