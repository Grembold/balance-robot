# Informationen zu Subversion mit Google Code #
## Überblick ##
GoogleCode verwendet Subversion als Versionsverwaltungstool für Dateien und Verzeichnisse. Subversion ist quasi der „Nachfolger“ von CVS. Viele der Konzepte und Ideen, aber auch die Syntax sind sehr ähnlich, so dass der Umstieg von CVS auf Subversion leicht fallen sollte. Zudem unterstützen die Web-GUI bzw. separate Tools den Umgang erheblich.
Im Folgenden wird ein Überblick über Subversion und dessen Einsatz für drupal-de gegeben.
## Grundlegende Funktion ##
Die zu versionierenden Dateien und Verzeichnisse werden auf dem Subversion-Server (code.google.com) in einem projekt-spezifischen Repository (code.google.com/p/drupal-de/) vorgehalten. Der Zugriff auf das Repository erfolgt durch einen Subversion-Client über das Protokoll http: und die eindeutige URL der Datei bzw. des Verzeichnisses. Über einen zusätzlichen Parameter (-r) kann eine dedizierte Version angesprochen werden.
Dateien können zur Bearbeitung auf dem lokalen Rechner ausgecheckt (checkout) werden. Das lokale Arbeitsverzeichnis steht unter der Kontrolle von Subversion (siehe Verzeichnis .svn). Nun können beliebige Änderungen an den lokalen Kopien vorgenommen werden, aber auch neue Dateien hinzugefügt oder bestehende gelöscht werden. Ist man mit der lokalen Arbeit fertig, können sämtliche Änderungen ins Repository eingecheckt (commit) werden. Änderungen, die seit dem letzten Checkout bzw. Update im Repository erfolgt sind, lassen sich im lokalen Arbeitsverzeichnis mittels update nachziehen.
Modifizierende Zugriffe auf das Repository erfolgen stets über das Protokoll https: und es muss ein sinnvoller Kommentar (--message "das wurde geändert …") sowie der User (--username "_GoogleAccountName_") inkl. Passwort (--password "_googlecode.com password_") angegeben werden.
Unter einem Export (export) versteht man dagegen das Erstellen einer lokalen Kopie der Dateien und Verzeichnisse ohne Kontrolle durch Subversion (i.e. kein Verzeichnis .svn).
Subversion bietet eine ganze Reihe von Befehlen. Einen Überblick erhält man mit svn --help, Hilfe zu einem speziellen Command mittels svn --help _command_.
## Zugriffsmethoden ##
Auf das Repository kann auf verschiedene Art und Weise zugegriffen werden:
  * Über den CommandLine-Subversion-Client svn.
  * Über einen Browser in einfacher Struktur á la ftp-Listing: http://drupal-de.googlecode.com/svn/.
  * Über die komfortable GoogleCode-Web-GUI: http://code.google.com/p/drupal-de/ mit Zugriff auf das Wiki, Issues und die Sourcen.
  * Über separate (oftmals integrierte) Tools (z.B. TortoiseSVN als Windows-Explorer-Extension).
## Repository-Struktur ##
URL des Repositories: http[s](s.md)://balance-robot.googlecode.com/svn
Struktur:
  * branches Projektzweigen (hier wird kurz mal was ausprobiert)
  * trunk Projektstamm (Hier wird normal am Code gearbeitet)
  * tag Release Versionen (versionen die "makiert" wurden, zum Beispiel V1.0)