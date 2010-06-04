package net.wsgs.tichu;
import static org.junit.Assert.assertEquals;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import net.wsgs.model.karte.Karte;
import net.wsgs.process.KartenManager;

import org.junit.Test;

public class KartenManagerTest {

	private static final KartenManager kartenManager = new KartenManager();

	@Test
	public void testGetAllowKartenkombinatinenZwillinge() {
		// ein gesammtes KartenDeck holen
		List<Karte> deck = kartenManager.getGesamtesKartenDeck();

		// Alle möglichen Kartenkombinationen ermitteln
		List<List<Karte>> kombinationen = kartenManager
				.getAllowKartenkombinationen(deck);

		// Nur Kombinationen betrachen die zwei Karten beinhalten
		Iterator<List<Karte>> iterator = kombinationen.iterator();
		List<List<Karte>> kombinationenMitZweiKarten = new ArrayList<List<Karte>>();
		while (iterator.hasNext()) {
			List<Karte> karten = iterator.next();
			if (karten.size() == 2) {
				kombinationenMitZweiKarten.add(karten);
				for(Karte k3 : karten){
					System.out.print(k3.toString() + "  --  " );
				}
				System.out.println("");
			}
		}

		/*
		 * Prüfen ob auch alles richtig ist!
		 */
		// Anzahl der Pärchen prüfen. 6 Paare pro Wert * 13 verschiedene Werte +
		// 13 mit Phoenix
		assertEquals(130, kombinationen.size());
	}
}
