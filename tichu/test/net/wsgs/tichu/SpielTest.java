package net.wsgs.tichu;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import net.wsgs.model.karte.Karte;
import net.wsgs.model.karte.Stich;
import net.wsgs.model.spieler.Spieler;
import net.wsgs.model.spieler.SpielerAnsage;
import net.wsgs.model.spieler.Team;

import org.junit.Test;

/**
 * Test Klasse zum aufbauen des Spiels
 * 
 * @author Arne
 * 
 */
public class SpielTest {

	@Test
	public void testSpiel() {
		
		generateSpielerAndTeams();

	}

	private List<Spieler> generateSpielerAndTeams() {
		Team teamA = new Team("TeamA", 0);
		Team teamB = new Team("TeamB", 0);

		List<Spieler> spieler = new ArrayList<Spieler>();
		spieler.add(new Spieler("Hans", teamA, new ArrayList<Karte>(),
				SpielerAnsage.KEINE, new ArrayList<Stich>()));
		spieler.add(new Spieler("Franz", teamB, new ArrayList<Karte>(),
				SpielerAnsage.KEINE, new ArrayList<Stich>()));
		spieler.add(new Spieler("Klaus", teamA, new ArrayList<Karte>(),
				SpielerAnsage.KEINE, new ArrayList<Stich>()));
		spieler.add(new Spieler("Maus", teamB, new ArrayList<Karte>(),
				SpielerAnsage.KEINE, new ArrayList<Stich>()));

		return spieler;
	}
}
