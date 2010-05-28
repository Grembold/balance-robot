package net.wsgs.model.spiel;

import net.wsgs.model.spieler.Team;

public class Spiel {
	private Spielpartie spielpartie;
	private Spielphase spielphase;
	private Team team1;
	private Team team2;
	private int punkteTeam1;
	private int punkteTeam2;

	public Spiel(Spielpartie spielpartie, Spielphase spielphase, Team team1,
			Team team2, int punkteTeam1, int punkteTeam2) {
		super();
		this.spielpartie = spielpartie;
		this.spielphase = spielphase;
		this.team1 = team1;
		this.team2 = team2;
		this.punkteTeam1 = punkteTeam1;
		this.punkteTeam2 = punkteTeam2;
	}

	public Spielpartie getSpielpartie() {
		return spielpartie;
	}

	public void setSpielpartie(Spielpartie spielpartie) {
		this.spielpartie = spielpartie;
	}

	public Spielphase getSpielphase() {
		return spielphase;
	}

	public void setSpielphase(Spielphase spielphase) {
		this.spielphase = spielphase;
	}

	public Team getTeam1() {
		return team1;
	}

	public void setTeam1(Team team1) {
		this.team1 = team1;
	}

	public Team getTeam2() {
		return team2;
	}

	public void setTeam2(Team team2) {
		this.team2 = team2;
	}

	public int getPunkteTeam1() {
		return punkteTeam1;
	}

	public void setPunkteTeam1(int punkteTeam1) {
		this.punkteTeam1 = punkteTeam1;
	}

	public int getPunkteTeam2() {
		return punkteTeam2;
	}

	public void setPunkteTeam2(int punkteTeam2) {
		this.punkteTeam2 = punkteTeam2;
	}

}
