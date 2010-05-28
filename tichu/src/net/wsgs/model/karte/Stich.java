package net.wsgs.tichu.model.karte;

import java.util.List;


public class Stich {

	private int punkte;
	private List<Karte> karten;

	public Stich(int punkte, List<Karte> karten) {
		super();
		this.punkte = punkte;
		this.karten = karten;
	}

	public int getPunkte() {
		return punkte;
	}

	public void setPunkte(int punkte) {
		this.punkte = punkte;
	}

	public List<Karte> getKarten() {
		return karten;
	}

	public void setKarten(List<Karte> karten) {
		this.karten = karten;
	}

}
