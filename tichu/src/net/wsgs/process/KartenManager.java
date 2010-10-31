package net.wsgs.process;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

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

		// Alle m�glichen Zwillinge ermitteln
		erlaubteZwillingeErmitteln(karten, alleErlaubtenKombinationen);

		// TODO Erlaubte Drillinge hinzuf�gen
		erlaubteDrillingeErmitteln(karten, alleErlaubtenKombinationen);
		// TODO Erlaubte FullHouse hinzuf�gen
		// TODO Erlaubte Stra�en hinzuf�gen
		// TODO Erlaubte Treppen hinzuf�gen

		return alleErlaubtenKombinationen;
	}

	/**
	 * TODO Tobi!!
	 * 
	 * @param karten
	 * @param alleErlaubtenKombinationen
	 */
	private void erlaubteDrillingeErmitteln(List<Karte> karten,
			List<List<Karte>> alleErlaubtenKombinationen) {
		if (karten.size() > 14) {
			System.err
					.println("Es konnten nicht alle Drillinge ermittelt werden");
			return;
		}
		Sonderkarte phoenix = null;
		Map<Integer, List<Karte>> sortierstapel = new HashMap<Integer, List<Karte>>();

		for (Karte k : karten) {
			// Wir hatten schon mal eine Karte mit diesem Wert
			if (sortierstapel.containsKey(k.getWert())) {
				List<Karte> stapel = sortierstapel.get(k.getWert());
				stapel.add(k);
			} else {
				// Wir hatten noch keine Karte mit diesem Wert
				List<Karte> neuerStapel = new ArrayList<Karte>();
				neuerStapel.add(k);
				sortierstapel.put(k.getWert(), neuerStapel);
			}

		}
		// Hatten wir einen Ph�nix?
		if (sortierstapel.containsKey(Sonderkarte.PHOENIX_WERT)) {
			phoenix = (Sonderkarte) sortierstapel.get(Sonderkarte.PHOENIX_WERT)
					.get(0);
		}
		// Sortierstapel durchgehen
		for (List<Karte> stapel : sortierstapel.values()) {
			// Ohne Ph�nix
			if (phoenix == null) {
				if (stapel.size() == 3) {
					// f�r 3 Karten
					alleErlaubtenKombinationen.add(stapel);
				} else if (stapel.size() == 4) {
					// f�r 4 Karten
					List<Karte> dieKombination = new ArrayList<Karte>();
					for (int i = 0; i < 4; i++) {
						dieKombination.addAll(stapel);
						dieKombination.remove(i);
						alleErlaubtenKombinationen.add(dieKombination);
					}
				}
			} else {
				// Mit Ph�nix
				stapel.add(phoenix);
				if (stapel.size() == 3) {
					alleErlaubtenKombinationen.add(stapel);
				} else if (stapel.size() == 4) {
					List<Karte> dieKombination = new ArrayList<Karte>();
					for (int i = 0; i < 4; i++) {
						dieKombination.addAll(stapel);
						dieKombination.remove(i);
						alleErlaubtenKombinationen.add(dieKombination);
					}
				} else if (stapel.size() == 5) {
for (int x = 0; x < 5; x++){
	for (int y = 0; y < 4 ; y++){
		
	}
}
				}
			}
		}

	}

	/**
	 * Ermitteln alle erlaubten Paare/Zwillinge und f�gt diese der Liste aller
	 * erlaubten Kartenkombinationen hinzu. Die �bergebene Karten m�ssen
	 * sortiert sein.
	 * 
	 * @param karten
	 * @param alleErlaubtenKombinationen
	 */
	private void erlaubteZwillingeErmitteln(List<Karte> karten,
			List<List<Karte>> alleErlaubtenKombinationen) {
		Sonderkarte phoenix = null;

		// Alle m�glichen Zwillinge ermitteln
		List<Karte> kartenMitGleichenWert = new ArrayList<Karte>();
		for (Karte karte : karten) {
			if (karte.getWert() == Sonderkarte.PHOENIX_WERT) {
				phoenix = (Sonderkarte) karte;
			}

			// Alle Karten mit gleiche Wert zusammensammeln
			if (kartenMitGleichenWert.isEmpty()
					|| kartenMitGleichenWert.get(0).getWert() == karte
							.getWert())
				kartenMitGleichenWert.add(karte);
			else {

				// Karten vom gleichem Wert durchlaufen
				int kartenAnzahl = kartenMitGleichenWert.size();
				for (int indexKarte1 = 0; indexKarte1 < kartenAnzahl; indexKarte1++) {
					Karte k1 = kartenMitGleichenWert.get(indexKarte1);
					for (int indexKarte2 = indexKarte1; indexKarte2 < kartenAnzahl; indexKarte2++) {
						Karte k2 = kartenMitGleichenWert.get(indexKarte2);

						// Keine Kombination mit sich selber m�glich
						if (k2.getFarbe() == null || k1.getFarbe() == null
								|| k2.getFarbe().equals(k1.getFarbe()))
							continue;

						List<Karte> paarKombination = new ArrayList<Karte>(2);
						paarKombination.add(k1);
						paarKombination.add(k2);

						alleErlaubtenKombinationen.add(paarKombination);
					}
				}
				kartenMitGleichenWert = new ArrayList<Karte>();
				kartenMitGleichenWert.add(karte);
			}
		}

		if (phoenix != null) {
			for (Karte karte : karten) {
				if (karte instanceof Sonderkarte)
					continue;

				List<Karte> paarKombination = new ArrayList<Karte>(2);
				paarKombination.add(karte);
				paarKombination.add(phoenix);

				alleErlaubtenKombinationen.add(paarKombination);
			}
		}
	}

	public List<Karte> getGesamtesKartenDeck() {
		List<Karte> alleKarten = new ArrayList<Karte>();
		/*
		 * F�r alle KartenFarben eine Karte mit den Werte von 2 bis 14
		 * anlengen.<br> Die folgenden Codezeilen k�nnte man in prosa wie folgt
		 * aus sprechen.<br> Durchlaufe alle Werte die es als Kartenfabe gibt.
		 * Die jeweilige Farbe sei <code>farbe</code>.<br> Durchlaufe die Werte
		 * von 2 bis 14. Der jeweilige Wert sei i.<br> F�ge der Liste
		 * allerKarten eine neue Karte in der Farbe und dem Wert hinzu.
		 */
		for (Kartenfarbe farbe : Kartenfarbe.values()) {
			for (int i = 2; i <= 14; i++) {
				alleKarten.add(new Karte(farbe, i));
			}
		}

		// Sonderkarten hinzuf�gen
		alleKarten.add(new Sonderkarte(SonderkartenTyp.HUND));
		alleKarten.add(new Sonderkarte(SonderkartenTyp.MAHJONGG));
		alleKarten.add(new Sonderkarte(SonderkartenTyp.DRACHE));
		alleKarten.add(new Sonderkarte(SonderkartenTyp.PHOENIX));

		return alleKarten;
	}
}
