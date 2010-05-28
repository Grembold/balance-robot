package net.wsgs.tichu.model.karte;

/**
 * Diese Klasse repräsentiert eine Spielkarte
 * 
 * @author Arne
 */
public class Karte {

	private Kartenfarbe farbe;
	private int wert;
	private int punkte;

	public Karte(Kartenfarbe farbe, int wert, int punkte) {
		super();
		this.farbe = farbe;
		this.wert = wert;
		this.punkte = punkte;
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

}
