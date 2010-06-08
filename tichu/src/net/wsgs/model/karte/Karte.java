package net.wsgs.model.karte;

/**
 * Diese Klasse repräsentiert eine Spielkarte
 * 
 * @author Arne
 */
public class Karte implements Comparable<Karte> {

	private Kartenfarbe farbe;
	private int wert;
	private int punkte;

	public Karte(Kartenfarbe farbe, int wert) {
		super();
		this.farbe = farbe;
		this.wert = wert;

		if (wert == 5) {
			this.punkte = 5;
		} else if (wert == 10 || wert == 13) {
			this.punkte = 10;
		} else {
			this.punkte = 0;
		}
	}

	public Kartenfarbe getFarbe() {
		return farbe;
	}

	public void setFarbe(Kartenfarbe farbe) {
		this.farbe = farbe;
	}

	public int getWert() {
		return wert;
	}

	public void setWert(int wert) {
		this.wert = wert;
	}

	public int getPunkte() {
		return punkte;
	}

	public void setPunkte(int punkte) {
		this.punkte = punkte;
	}

	public boolean equals(Object other) {
		if (other instanceof Karte) {
			Karte k = (Karte) other;

			return k.wert == this.wert && k.farbe == this.farbe;
		}
		return false;
	}

	public String toString() {
		return this.wert + "(" + farbe.toString() + ")";
	}

	public int compareTo(Karte other) {
		return this.wert - other.wert;
	}

}
