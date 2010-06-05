package net.wsgs.tichu;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import net.wsgs.model.karte.Karte;
import net.wsgs.process.KartenManager;

import org.junit.Test;

public class KartenManagerTest {

	private static final KartenManager kartenManager = new KartenManager();

	@Test
	public void testGetGesamtesKartenDeck() {
		// ein gesammtes KartenDeck holen
		List<Karte> deck = kartenManager.getGesamtesKartenDeck();

		// Pr�fung ob im Deck 56 Karten enthalten sind
		assertEquals(56, deck.size());
	}

	@Test
	public void testGetAllowKartenkombinatinenZwillinge() {
		// ein gesammtes KartenDeck holen
		List<Karte> deck = kartenManager.getGesamtesKartenDeck();

		// Alle m�glichen Kartenkombinationen ermitteln
		List<List<Karte>> kombinationen = kartenManager
				.getAllowKartenkombinationen(deck);

		// Nur Kombinationen betrachen die zwei Karten beinhalten
		Iterator<List<Karte>> iterator = kombinationen.iterator();
		List<List<Karte>> kombinationenMitZweiKarten = new ArrayList<List<Karte>>();
		while (iterator.hasNext()) {
			List<Karte> karten = iterator.next();
			if (karten.size() == 2) {
				kombinationenMitZweiKarten.add(karten);
				System.out.println(String.format("%14s %14s", 
						karten.get(0).toString(), 
						karten.get(1).toString()));
			}
		}

		/*
		 * Pr�fen ob auch alles richtig ist!
		 */
		// Anzahl der P�rchen pr�fen. 6 Paare pro Wert * 13 verschiedene Werte +
		// 13 mit Phoenix
		assertEquals(130, kombinationenMitZweiKarten.size());

		// Es d�rfen keine P�rchen existieren, wo beide Karten die gleiche Farbe
		// haben.
		for (List<Karte> kombi : kombinationenMitZweiKarten) {
			assertTrue(!(kombi.get(0).getFarbe() == kombi.get(1).getFarbe()));
		}
	}

	@Test
	public void testGetAllowKartenkombinatinenDrillinge() {
		// ein gesammtes KartenDeck holen
		List<Karte> deck = kartenManager.getGesamtesKartenDeck();

		// Alle m�glichen Kartenkombinationen ermitteln
		List<List<Karte>> kombinationen = kartenManager
				.getAllowKartenkombinationen(deck);

		// Nur Kombinationen betrachen die drei Karten beinhalten
		Iterator<List<Karte>> iterator = kombinationen.iterator();
		List<List<Karte>> kombinationenMitDreiKarten = new ArrayList<List<Karte>>();
		while (iterator.hasNext()) {
			List<Karte> karten = iterator.next();
			if (karten.size() == 3) {
				kombinationenMitDreiKarten.add(karten);
				System.out.println(String.format("%14s %14s %14s", 
						karten.get(0).toString(), 
						karten.get(1).toString(), 
						karten.get(2).toString()));
			}
		}

		/*
		 * Pr�fen ob auch alles richtig ist!
		 */
		// Anzahl der Drillinge pr�fen. 4 Drillinge pro Wert * 13 verschiedene
		// Werte +
		// 78 Zwillinge mit Phoenix
		assertEquals(130, kombinationenMitDreiKarten.size());
	}

	
	@Test
	public void testGetAllowKartenkombinatinenVierlinge() {
		// ein gesammtes KartenDeck holen
		List<Karte> deck = kartenManager.getGesamtesKartenDeck();

		// Alle m�glichen Kartenkombinationen ermitteln
		List<List<Karte>> kombinationen = kartenManager
				.getAllowKartenkombinationen(deck);

		// Nur Kombinationen betrachen die drei Karten beinhalten
		Iterator<List<Karte>> iterator = kombinationen.iterator();
		List<List<Karte>> kombinationenMitDreiKarten = new ArrayList<List<Karte>>();
		while (iterator.hasNext()) {
			List<Karte> karten = iterator.next();
			if (karten.size() == 4) {
				kombinationenMitDreiKarten.add(karten);
				System.out.println(String.format("%14s %14s %14s %14s", 
						karten.get(0).toString(), 
						karten.get(1).toString(), 
						karten.get(2).toString(),
						karten.get(3).toString()));
			}
		}

		/*
		 * Pr�fen ob auch alles richtig ist!
		 */
		// Anzahl der Vierlinge pr�fen. 1 Vierling pro Wert * 13 verschiedene
		// Werte + 52 Drillinge mit Phoenix
		assertEquals(65, kombinationenMitDreiKarten.size());
	}
}
