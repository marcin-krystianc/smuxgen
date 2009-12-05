--------------------------------------------------------------------------------
			SMUXGEN - SuperMemo UX Generator
--------------------------------------------------------------------------------
Version 2.0 (Tested with SuperMemo UX 1.4.3.2)
Author	Marcin Krystianc (marcin.krystianc@gmail.com)
OS	Windows
License GPL
URL 	http://code.google.com/p/smuxgen/
--------------------------------------------------------------------------------



1. Opis programu.

SMUXGEN jest darmowym programem służącym do ułatwienia procesu tworzenia własnych 
kursów w programie SuperMemo UX. Kursy generowane są na podstawie pliku tekstowego.
Dodatkowo istnieje możliwość automatycznego pobierania obrazków z internetu 
i generowania pliku mp3 z lektorem. 

2. Metoda SuperMemo, czyli dlaczego warto (http://www.supermemo.pl/opis_metody).

SuperMemo to wspomagana komputerowo metoda uczenia się opracowana na bazie 
długoletnich badań nad pamięcią długotrwałą i czynnikami odpowiedzialnymi 
za skuteczne zapamiętywanie nowych informacji. Głównym założeniem metody jest 
zapewnienie osobie uczącej się optymalnych odstępów pomiędzy poszczególnymi 
powtórkami materiału. Codzienna półgodzinna nauka pozwala przyswoić podstawowy 
zasób słów z dowolnego języka obcego (3000 wyrazów, związków frazeologicznych 
i form gramatycznych) w czasie nie dłuższym niż jeden miesiąc.

Program SuperMemo stale analizuje poziom zapominania i zapamiętywania materiału,
co pozwala na dostosowanie planu powtórek do indywidualnych potrzeb każdego ucznia.
Dzięki matematycznie zaawansowanym algorytmom, dla każdej informacji 
zawartej w kursach SuperMemo tworzony jest oddzielny plan powtórek. 
Uczeń nie traci zatem czasu na powtarzanie wiadomości, które dobrze pamięta,
za to z większą intensywnością ćwiczy materiał, który stwarza mu większe problemy.

3. Instalacja.

Program SMUXGEN nie wymaga instalacji, dlatego jest rozprowadzany tylko w postaci 
archiwum 7z wraz z wszystkimi komponentami. W przypadku chęci korzystania z opcji mp3,
należy zainstalować odpowiednie engine "text to speach". SMUXGEN opiera się 
o standardowy interfejs "Microsoft Speach API".

W skład programu wchodzi:

  katalog bin:
    smuxgen.exe         - główny komponent programu SMUXGEN
    createMp3.bat 	- skryot odpowiedzialny za generowanie plików mp3
			  (można go edytować w celu dostosowania do potrzeb 
			  zainstalowanego engine "text to speach"
    getGoogleHtml.bat 	- skrypt pobierający wyniki wyszukiwania z serwisie Google images
    getImage.bat	- skrypt pobierający obrazki z podanego adresu URL

    lame.exe		- kodek mp3 (http://www.free-codecs.com/download/Lame_Encoder.htm)
    sapi2wav.exe	- darmowy program wykorzystujący interfejs SAPI
                          do generowania próbek z lektorem (http://blog.hacker.dk/)
    sox.exe		- manipulacja plikami dźwiękowymi (http://sox.sourceforge.net/)
    wget.exe		- pobierania plków (http://www.gnu.org/software/wget/)

    *.dll		- potrzebne biblioteki

  katalog dat:
   testowy.smuxgen      - przykładowy plik tekstowy dla kursu w języku niemieckim

 smuxgen.bat		- uruchamia program smuxgen
 smuxgen (batch).bat	- przykład uruchamiania programu smuxgen w trybie wsadowym, umożliwia automatyczne przetworzenie wielu plików

 readme.txt		- plik który właśnie czytasz

4. Tworzenie nowego kursu.

  a. Zainstaluj program SuperMemo UX (http://www.supermemo.pl/)
  b. Pobierz aktualizacje (BEZ AKTUALIZACJI WYGENEROWANE KURSY MOGĄ NIE DZIAŁAĆ, testowane z wersją 1.4.3.2)
  c. Utwórz nowy kurs w programie SuperMemo (kurs->dodaj->utwórz)
     W katalogu "C:\Program Files\SuperMemo UX\Courses\" zostanie stworzony katalog z nowym kursem.
  d. uruchom program smuxgen (zobacz smuxgen.bat)
  e. wzorując się na przykładzie, utwórz słówka dla nowego kursu
  f. zapisz kurs i kliknij przycisk generuj
  f. Po dodaniu nowych słówek, lub zmianie istniejęcyh kliknij ponownie generuj
     
5. Opcje.
  
  Regenerate 	- wumusza powtórne wygenerowanie wszystkich ćwiczeń
  Alternate 	- generuje kurs lustrzany
  Images	- dodaje obrazki do ćwiczeń
  Lector	- dodaje lektora do ćwiczeń
  
 
  Course Template:
  W kolejnych linijkach znajdują się słówka według schematu A:B|C
	A   - słówko w języku polskim
	B,C - alternatywne odpowiedzi w języku obcym

  Przykłady:
   	w końcu(wreszcie):schließlich
	syrop na kaszel:der Hustensaft|der Hustensirup

6. Mp3 z lektorem.

SMUXGEN pozwala na generowania automatyczne mp3 z lektorem przy użyciu interfejsu SAPI.
(np. darmowe demo http://cepstral.com/). Po zainstalowaniu takiego silnika,
powinien on być widoczny do wyboru w panel sterowania->mowa.
Skrypt createMp3.bat jest odpowiedzialny za:
	- wygenerowanie odpowiedniego pliku wav dla danego słówka
	- ewentualny postrpocessing pliku wav (np. obcięcie, zwiększenie głośności)
	- konwertowanie wav na mp3

Opcje lektora:

  Voice index	- numer silnika SAPI
  Gain		- wzmocnienie w decybelach
  Trim		- obcięcie w sekundach od początku pliku
   
  Kliknięcie w przycisk testu, powinno spowodować odegranie tekstu wpisanego w pole poniżej.	  

 
Oczywiście jakość próbek z lektorem zależy tylko i wyłącznie od jakości użytego silnika SAPI.


		


	

		

	 

  
    

  








		
 





 