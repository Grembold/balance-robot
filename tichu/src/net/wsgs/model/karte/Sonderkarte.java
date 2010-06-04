package net.wsgs.model.karte;

/**
 * Diese Klasse erweitert eine normale Karte zu einer Sonderkarte
 * 
 * Sonderkarten:<br>
 * <li>Der Hund hat den Wert 0</li> <li>Der Mahjongg hat den Wert 1</li> <li>Der
 * Phönix hat den Wert 15</li> <li>Der Drache hat den Wert 16</li>
 * 
 * @author Arne
 * 
 */
public class Sonderkarte extends Karte {

	public static final int HUND_WERT = 0;
	public static final int MAHJONGG_WERT = 1;
	public static final int PHOENIX_WERT = 15;
	public static final int DRACHEN_WERT = 16;

	private SonderkartenTyp typ;

	public Sonderkarte(SonderkartenTyp typ) {
		super(null, 0);
		this.typ = typ;
		
		// Kartenwert setzen
		switch (typ) {
		case HUND:
			setPunkte(0);
			setWert(HUND_WERT);
			break;
		case DRACHE:
			setPunkte(25);
			setWert(DRACHEN_WERT);
			break;
		case PHOENIX:
			setPunkte(-25);
			setWert(PHOENIX_WERT);
			break;
		case MAHJONGG:
			setPunkte(0);
			setWert(MAHJONGG_WERT);
			break;
		default:
			throw new RuntimeException("Unbekannter KartenTyp");
		}
	}

	public SonderkartenTyp getTyp() {
		return typ;
	}

	public void setTyp(SonderkartenTyp typ) {
		this.typ = typ;
	}
	
	public String toString(){
		return this.typ.toString();
	}

}
