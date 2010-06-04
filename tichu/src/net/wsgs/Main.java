package net.wsgs;

import java.util.ArrayList;
import java.util.List;

import net.wsgs.model.spiel.Spielpartie;
import net.wsgs.model.spieler.Spieler;
import net.wsgs.model.spieler.Team;

public class Main {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		
		Team teamA = new Team("Die Guten", 0);
		Team teamB = new Team("Die Schlechten", 0);
		
		Spieler spieler1 = new Spieler("Hans", teamA);
		Spieler spieler2 = new Spieler("Otto", teamB);
		
		List<Spieler> spielerListe = new ArrayList<Spieler>();
		spielerListe.add(spieler1);
		spielerListe.add(spieler2);
		Spielpartie dasSpiel = new Spielpartie();
		dasSpiel.setSpieler(spielerListe);
	}
	
}
