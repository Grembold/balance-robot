package net.wsgs.process;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JFrame;

import net.wsgs.tichu.Splash;

public class Spiel extends JFrame {
	private static final long serialVersionUID = -6949360314291548370L;

	public Spiel() {
		JButton connect = new JButton("Connect");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
		setLocation((d.width - getSize().width) / 2,
				(d.height - getSize().height) / 2);
		setSize(400, 300);
		
		// Alle Sternkarten als Button anlegen
		List<JButton> sternKarten = new ArrayList<JButton>();
		for (int i = 2; i<15; i++){
			byte[] b = { (byte)(64+(i%10)) };
			String name =  (i < 11 ? i+"" : new String(b)) ;
			JButton karte = new JButton(name+"★");
			karte.setBackground(Color.black);
			karte.setForeground(Color.RED);
			this.add(karte);
		}
		this.setLayout(new GridLayout(2, 2, 6, 3));
		this.add(connect);
		this.setTitle("Login");
	}

	public static void main(String[] args) throws InterruptedException {
		Splash splash = new Splash();
		splash.showSplash(true);
		Thread.sleep(2500);
		Spiel spiel = new Spiel();
		splash.showSplash(false);
		spiel.setVisible(true);
	}
}
