package net.wsgs.model.spieler;

import java.util.List;

import net.wsgs.model.karte.Karte;
import net.wsgs.model.karte.Stich;

public class Spieler {

	private String name;
	private Team team;
	private List<Karte> karten;
	private SpielerAnsage ansage;
	private List<Stich> stiche;

	public Spieler(String name, Team team, List<Karte> karten,
			SpielerAnsage ansage, List<Stich> stiche) {
		super();
		this.name = name;
		this.team = team;
		this.karten = karten;
		this.ansage = ansage;
		this.stiche = stiche;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public Team getTeam() {
		return team;
	}

	public void setTeam(Team team) {
		this.team = team;
	}

	public List<Karte> getKarten() {
		return karten;
	}

	public void setKarten(List<Karte> karten) {
		this.karten = karten;
	}

	public SpielerAnsage getAnsage() {
		return ansage;
	}

	public void setAnsage(SpielerAnsage ansage) {
		this.ansage = ansage;
	}

	public List<Stich> getStiche() {
		return stiche;
	}

	public void setStiche(List<Stich> stiche) {
		this.stiche = stiche;
	}

}
