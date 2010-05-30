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
			setWert(0);
			break;
		case DRACHE:
			setWert(25);
			break;
		case PHOENIX:
			setWert(-25);
			break;
		case MAHJONGG:
			setWert(0);
			break;
		default:
			throw new RuntimeException("Unbekannter KartenTyp");
		}
		
		//TODO KartenwPunkte setzten
	}

	public SonderkartenTyp getTyp() {
		return typ;
	}

	public void setTyp(SonderkartenTyp typ) {
		this.typ = typ;
	}

}
