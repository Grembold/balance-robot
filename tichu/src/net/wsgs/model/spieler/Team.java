package net.wsgs.tichu.model.spieler;

public class Team {

	private String name;
	private int punkte;

	public Team(String name, int punkte) {
		super();
		this.name = name;
		this.punkte = punkte;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public int getPunkte() {
		return punkte;
	}

	public void setPunkte(int punkte) {
		this.punkte = punkte;
	}

}
