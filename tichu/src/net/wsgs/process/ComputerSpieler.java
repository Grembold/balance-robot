package net.wsgs.process;

import java.util.List;

import net.wsgs.model.karte.Karte;

/**
 * Dieses Interface definiert die von einem Computergegner zu erfüllenden
 * Funktionen
 * 
 * @author Arne
 * 
 */
public interface ComputerSpieler {

	/**
	 * Liefert den Namen eines Spielers!
	 * 
	 * @return
	 */
	public String getName();

	/**
	 * Der Spieler ist ander Reihe und ist aufgefordert seinen Zug zu machen.
	 * 
	 * @return Liste der Katen die der Spieler in diesem Zug Spielen möchte. Ist
	 *         die Liste leer so passt der Spieler
	 */
	public List<Karte> macheDeinenZug();

	/**
	 * Der Spieler wird gefragt ob er eine Bombe werfen möchte. Diese Frage
	 * immer an den Spieler gestellt sobalt ein Spieler einen Zug gemacht hat.
	 * 
	 * @return Liste der Karten die der Spieler als Bombe werfen möchte. Ist die
	 *         Liste leer so wirft der Spieler keine Bombe.
	 */
	public List<Karte> bombeWerfen();
	
}
