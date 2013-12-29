--------------------------------------------------------------------------------
			SMUXGEN - SuperMemo UX Generator
--------------------------------------------------------------------------------
Version 2.22 (Tested with SuperMemo UX 1.5.1.13)
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
  b. Pobierz aktualizacje (BEZ AKTUALIZACJI WYGENEROWANE KURSY MOGĄ NIE DZIAŁAĆ)
  c. Utwórz nowy kurs w programie SuperMemo (kurs->dodaj->utwórz)
     W katalogu "C:\Program Files\SuperMemo UX\Courses\" zostanie stworzony katalog z nowym kursem.
  d. Uruchom program smuxgen (zobacz smuxgen.bat)
  e. Wzorując się na przykładzie, utwórz słówka dla nowego kursu
  f. Wybierz użytkownika i nazwę kursu (patrz punkt Opcje)
  g. Zapisz kurs i kliknij przycisk GENERATE (Informacje diagnostyczne znajdziesz w okienku "Console")
  h. Po dodaniu nowych słówek, lub zmianie istniejęcyh kliknij ponownie przycisk GENERATE
     
5. Opcje.
  
  User: 	- Nazwa użytkownika w SuperMemo UX
  Course	- Nazwa kursu do którego chemy dodać aktualną lekcję  
  Alternate 	- generuje kurs lustrzany
  Images	- dodaje obrazki do ćwiczeń
  Lector	- dodaje lektora do ćwiczeń
  
 
  Word List:
  W kolejnych linijkach znajdują się pary słów do nauki:

	()	- wszystko co znajduje się w nawiasach okragłych będzie wyświetlane, ale nie będzie czytane przez lektora
	[] 	- wszystko co znajduje się w nawiasach kwadratowych będzie wyświetlane (bez nawiasów) i użyte jako słowo kluczowe do wyszukiwania obrazków
	{} 	- wszystko co znajduje się w nawiasach klamrowych nie będzie wyświetlane
	
  Przykłady:
   	w końcu(wreszcie):schließlich
	syrop na kaszel:der Hustensaft|der Hustensirup
	Wer ist das {[wasser]}?(Kind) :Das ist das Kind.
	
6. Mp3 z lektorem.

SMUXGEN pozwala na generowania automatyczne mp3 z lektorem przy użyciu interfejsu SAPI.
(np. http://nextup.com/TextAloud/SpeechEngine/voices.html). Po zainstalowaniu takiego silnika,
powinien on być widoczny do wyboru w panel sterowania->mowa.
Skrypt createMp3.bat jest odpowiedzialny za:
	- wygenerowanie odpowiedniego pliku wav dla danego słówka
	- ewentualny postrpocessing pliku wav (np. obcięcie, zwiększenie głośności)
	- konwertowanie wav na mp3

Opcje lektora:

  Voice 	- nazwa silnika SAPI
  Gain		- wzmocnienie w decybelach
  Trim		- obcięcie w sekundach od początku pliku
   
  Kliknięcie w przycisk testu, powinno spowodować odegranie tekstu wpisanego w pole poniżej.	  

 
Oczywiście jakość próbek z lektorem zależy tylko i wyłącznie od jakości użytego silnika SAPI.

7. Course Browser

Jest to funkcja pozwalająca na przeglądanie i zmianę obrazów dla już wygenerowanego kursu, oraz na przeglądanie plików mp3.
Nowe obrazki na bieżąco pobierane są z Internetu.
Gdy domyślne wyniki wyszukiwania są niezadowalające można użyć własnych słów kluczowych.
Obrazy podmienia się przeciągając je z listy po prawej na jeden z obrazów docelowych ("drag and drop").

8. Changelog

V 2.22		- Issue: images aren't downloaded
 		- Removed option: regenerate
		- Removed course browser
		- Word list redisigned (In future will replace picture browser)
V 2.21		- Issue: images aren't downloaded
V 2.20		- Issue: both images are the same
V 2.19		- Pop-up window for unsaved changes 
V 2.18		- Error correction for mp3 generation (words separated by comma)
V 2.18		- Improved progress information
V 2.18		- Better image scaling
V 2.17		- extended elements definition (),[],{}
V 2.16		- voice engine by name
V 2.15		- mp3 for Q&A , mp3 browser
V 2.14		- Possible to define instruction for course element
V 2.14 	 	- Improved editor
V 2.13 		- Picture Browser for single courses too
V 2.13 		- Import/Export Q&A		
V 2.12 		- SQL database as source of ID numbers
V 2.12 		- Picture Browser, QSplitter,unique images on the list
V 2.11 		- Picture Browser , recent files
V 2.1 		- Picture Browser (ready)
V 2.0.3 	- Some deleted items weren't removed from database (QSqlTableModel issue)
V 2.0.3 	- Better image handling (scaling and deleting)
V 2.0.2 	- Disable RichText generate.mp3 - added replace for "|" character disabled timeout in waiting for process end
V 2.0	 	- Support for SuperMemo Ux 1.4.3.2 (SQLite)
 