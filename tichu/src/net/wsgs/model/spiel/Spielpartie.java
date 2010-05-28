package net.wsgs.tichu.model.spiel;

import java.util.List;

import net.wsgs.tichu.model.spieler.Spieler;

public class Spielpartie {
	private List<Spieler> spieler;

	public List<Spieler> getSpieler() {
		return spieler;
	}

	public void setSpieler(List<Spieler> spieler) {
		this.spieler = spieler;
	}

}
