--------------------------------------------------------------------------------
				SMUXGEN
--------------------------------------------------------------------------------
Verion 	1.0
AUTOR	Marcin Krystianc (marcin.krystianc@gmail.com)
OS	Windows XP
License GPL
URL 	http://code.google.com/p/smuxgen/
--------------------------------------------------------------------------------



1. Opis programu.

SMUXGEN jest darmowym programem s�u��cym do u�atwienia procesu tworzenia w�asnych 
kurs�w w programie SuperMemo UX. Kursy generowane s� na podstawie pliku tekstowego.
Dodatkowo istnieje mo�liwo�� automatycznego pobierania obrazk�w z internetu 
i generowania pliku mp3 z lektorem. SMUXGEN nie posiada interfejsu graficznego,
jego dzia�anie opiera si� oprzetwarzanie wsadowe.

2. Metoda SuperMemoczyli dlaczego warto. (http://www.supermemo.pl/opis_metody)

SuperMemo to wspomagana komputerowo metoda uczenia si� opracowana na bazie 
d�ugoletnich bada� nad pami�ci� d�ugotrwa�� i czynnikami odpowiedzialnymi 
za skuteczne zapami�tywanie nowych informacji. G��wnym za�o�eniem metody jest 
zapewnienie osobie ucz�cej si� optymalnych odst�p�w pomi�dzy poszczeg�lnymi 
powt�rkami materia�u. Codzienna p�godzinna nauka pozwala przyswoi� podstawowy 
zas�b s��w z dowolnego j�zyka obcego (3000 wyraz�w, zwi�zk�w frazeologicznych 
i form gramatycznych) w czasie nie d�u�szym ni� jeden miesi�c.

Program SuperMemo stale analizuje poziom zapominania i zapami�tywania materia�u,
co pozwala na dostosowanie planu powt�rek do indywidualnych potrzeb ka�dego ucznia.
 Dzi�ki matematycznie zaawansowanym algorytmom, dla ka�dej informacji 
zawartej w kursach SuperMemo tworzony jest oddzielny plan powt�rek. 
Ucze� nie traci zatem czasu na powtarzanie wiadomo�ci, kt�re dobrze pami�ta,
za to z wi�ksz� intensywno�ci� �wiczy materia�, kt�ry stwarza mu wi�ksze problemy.

3. Instalacja.

Program SMUXGEN nie wymaga instalacji, dlateg ojest rozprowadzany tylko w postaci 
archiwum 7z wraz z wszystkimi komponentami.W przypadku ch�ci ko�ystania z opcji mp3,
nale�y zainstalowa� odpowiednie engine "text to speach". SMUXGEN opiera si� 
o standardowy interfejs "Microsoft Speach API" firmy microsoft.

W sk�ad programu wchodzi:

  katalog bin:
    smuxgen.exe         - g��wny komponent programu SMUXGEN
    createMp3.bat 	- skryot odpowiedzialny za generowanie plik�w mp3
			  (mozna go edytowa� w celu dostosowania do potrzeb 
			  zainstalowanego enginu "text to speach"
    getGoogleHtml.bat 	- skrypt pobieraj�cy wyniki wyszukiwania z serwise google images
    getImage.bat	- skrypt pobieraj�cy obrazki z podanego adresu URL

    lame.exe		- kodek mp3 (http://www.free-codecs.com/download/Lame_Encoder.htm)
    sapi2wav.exe	- darmowy program wyko�ystuj�cy interfejs SAPI
                          do generowania pr�bek z lektorem (http://blog.hacker.dk/)
    sox.exe		- manipulacja plikami d�wi�kowymi (http://sox.sourceforge.net/)
    wget.exe		- pobierania plk�w (http://www.gnu.org/software/wget/)

    *.dll		- potrzebne biblioteki

  katalog dat:
   testowy.smuxgen      - przyk�adowy plik tekstowy dla kursu w j�zyku niemieckim

 smuxgen.bat		- przyk�adowy skrypt startowy dla pliku testowy.smuxgen
 readme.txt		- plik kt�ry w�a�nie czytasz

4. Tworzenie nowego kursu.

  a. Zainstaluj program SuperMemo UX (http://www.supermemo.pl/)
  b. Pobierz aktualizacje
  c. Utw�rz nowy kurs w programie SuperMemo (kurs->dodaj->utw�rz)
     W katalogu "C:\Program Files\SuperMemo UX\Courses\" zostanie stworzony katalog z nowym kursem.
  d. Uw�rz plik kurs.smuxgen ze s�owkami do zapami�tania
  e. uruchom program smuxgen (zobacz smuxgen.bat)
  f. Po dodaniu nowych s��wek, lub zmianie istniej�cyh uruchom jeszcze raz smuxgen.
     
5. Strukt�ra pliku smuxgen z kursem.

Pliki ze s��wkami musz� by� zapisane w kodowaniu UTF-8!

  W pierwszej linijce znajduj� si� tylko i wy��cznie �cie�ka do pliku bazowego kursu(4c) np:
    C:\Program Files\SuperMemo UX\Courses\testowy\override\course.xml

  W drugiej linijce znajduje si�:
    NAZWA_LEKCJI parametry parametry parametry parametry

    NAZWA_LEKCJI - Nazwa lekcji pod kt�ra zostan� umieszczone s��wka do zapami�tania.
	           Jeden plik zawiera s��wka do jednej lekcji.
		   Kilka r�nych plik�w mo�e zawiera� s��wka do tego samego kursu.

    Parametry s�u�a do modyfikowania zachowania programu SMUXGEN. 
    Parametry aktywuje si� poprzez wypisanie ich dok�adnej nazwy w drugiej linijce.
    Deaktywacja nast�puj� przez modyfikacje nazwy np. dodanie znaku "#":

	Force - wymusza wygenerowania wszystkich �wicze� ponownie (wraz z mp3 i obrazkami)
	Double- tworzy dodatkowo kurs alternatywny (jezyk obcy -> jezyk polski)
	Voice - Tworzy pliki mp3 z lektorem (skrypt createMp3.bat)
	Image - Tworzy pliki jpg (getGoogleHtml.bat i getImage.bat)
	Clean - Usuwa z kursu �wiczenia kt�re zosta�y usuni�te z pliku ze s��wkami.
		(Mo�e spowodowa� problemy z dzia�aniem SuperMemo, opcja eksperymentalna)

  W kolejnych linijkach znajduj� si� s��wka wed�ug schematu A:B|C
	A   - s��wko w j�zyku polskim
	B,C - alternatywne odpowiedzi w j�zyku obcym

  Przyk�ady:
   	w ko�cu(wre�cie):schlie�lich
	syrop na kaszel:der Hustensaft|der Hustensirup

6. Mp3 z lektorem.

SMUXGEN pozwala na generowania automatyczne mp3 z lektorem przy u�yciu interfejsu SAPI.
(np. darmowe demo http://cepstral.com/). Po zainstalowaniu takiego silnika,
powinien on by� widoczny do wyboru w panel sterowania->mowa.
Skrypt createMp3.bat jest odpowiedzialny za:
	- wygenerowanie odpowiedniego pliku wav dla danego s��wka
	- ewentualny postrpocessing pliku wav (np. obci�cie, podg��nienie)
	- przekonwertowaniu wav na mp3
	
 
Oczywi�cie jako�� pr�bek z lektorem zale�y tylko i wy��cznie od jako�ci u�ytego silnika SAPI.


		


	

		

	 

  
    

  








		
 





 