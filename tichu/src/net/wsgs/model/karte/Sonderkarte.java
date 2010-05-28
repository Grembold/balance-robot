package net.wsgs.tichu.model.karte;

/**
 * Diese Klasse erweitert eine normale Karte zu einer Sonderkarte
 * 
 * @author Arne
 * 
 */
public class Sonderkarte extends Karte {

	private SonderkartenTyp typ;

	public Sonderkarte(Kartenfarbe farbe, int wert, int punkte,
			SonderkartenTyp typ) {
		super(farbe, wert, punkte);
		this.typ = typ;
	}

	public SonderkartenTyp getTyp() {
		return typ;
	}

	public void setTyp(SonderkartenTyp typ) {
		this.typ = typ;
	}

}
