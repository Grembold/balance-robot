package net.wsgs.util;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import net.wsgs.model.karte.Karte;

public class KartenUtil {

	public static String kartenListToString(final List<Karte> kombi) {
		if (kombi == null || kombi.isEmpty())
			return "LEER";
		String komiInStrng = "";
		for (int i = 0; i < kombi.size(); i++) {
			komiInStrng += String.format("%14s ", kombi.get(0).toString());
		}
		return komiInStrng;
	}

	/**
	 * Funktion prüft ob die Übergebenen Karten eine Strasse bilden.
	 * 
	 * @param kombi
	 * @return der Rückgabewert ist true wenn eine Strasse vorliegt
	 */
	public static boolean isStrasse(final List<Karte> kombi) {
		if (kombi == null || kombi.size() < 5)
			return false;

		// Pr�fen ob das eine Strasse ist
		Collections.sort(kombi);
		int ersterWert = 0;
		boolean isStrasse = true;
		for (int index = 0; index < kombi.size(); index++) {
			if (index == 0) {
				ersterWert = kombi.get(0).getWert();
				continue;
			}

			if (kombi.get(index).getWert() != ersterWert + index)
				isStrasse = false;
		}
		return isStrasse;
	}

	public static List<Karte> cloneKartenKombi(final List<Karte> kombi) {
		if (kombi == null || kombi.size() < 5)
			return Collections.emptyList();

		List<Karte> clone = new ArrayList<Karte>();

		for (Karte karte : kombi) {
			clone.add(new Karte(karte.getFarbe(), karte.getWert()));
		}
		return clone;
	}
}
