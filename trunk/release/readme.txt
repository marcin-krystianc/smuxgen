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

SMUXGEN jest darmowym programem s³u¿¹cym do u³atwienia procesu tworzenia w³asnych 
kursów w programie SuperMemo UX. Kursy generowane s¹ na podstawie pliku tekstowego.
Dodatkowo istnieje mo¿liwoœæ automatycznego pobierania obrazków z internetu 
i generowania pliku mp3 z lektorem. SMUXGEN nie posiada interfejsu graficznego,
jego dzia³anie opiera siê oprzetwarzanie wsadowe.

2. Metoda SuperMemoczyli dlaczego warto. (http://www.supermemo.pl/opis_metody)

SuperMemo to wspomagana komputerowo metoda uczenia siê opracowana na bazie 
d³ugoletnich badañ nad pamiêci¹ d³ugotrwa³¹ i czynnikami odpowiedzialnymi 
za skuteczne zapamiêtywanie nowych informacji. G³ównym za³o¿eniem metody jest 
zapewnienie osobie ucz¹cej siê optymalnych odstêpów pomiêdzy poszczególnymi 
powtórkami materia³u. Codzienna pó³godzinna nauka pozwala przyswoiæ podstawowy 
zasób s³ów z dowolnego jêzyka obcego (3000 wyrazów, zwi¹zków frazeologicznych 
i form gramatycznych) w czasie nie d³u¿szym ni¿ jeden miesi¹c.

Program SuperMemo stale analizuje poziom zapominania i zapamiêtywania materia³u,
co pozwala na dostosowanie planu powtórek do indywidualnych potrzeb ka¿dego ucznia.
 Dziêki matematycznie zaawansowanym algorytmom, dla ka¿dej informacji 
zawartej w kursach SuperMemo tworzony jest oddzielny plan powtórek. 
Uczeñ nie traci zatem czasu na powtarzanie wiadomoœci, które dobrze pamiêta,
za to z wiêksz¹ intensywnoœci¹ æwiczy materia³, który stwarza mu wiêksze problemy.

3. Instalacja.

Program SMUXGEN nie wymaga instalacji, dlateg ojest rozprowadzany tylko w postaci 
archiwum 7z wraz z wszystkimi komponentami.W przypadku chêci ko¿ystania z opcji mp3,
nale¿y zainstalowaæ odpowiednie engine "text to speach". SMUXGEN opiera siê 
o standardowy interfejs "Microsoft Speach API" firmy microsoft.

W sk³ad programu wchodzi:

  katalog bin:
    smuxgen.exe         - g³ówny komponent programu SMUXGEN
    createMp3.bat 	- skryot odpowiedzialny za generowanie plików mp3
			  (mozna go edytowaæ w celu dostosowania do potrzeb 
			  zainstalowanego enginu "text to speach"
    getGoogleHtml.bat 	- skrypt pobieraj¹cy wyniki wyszukiwania z serwise google images
    getImage.bat	- skrypt pobieraj¹cy obrazki z podanego adresu URL

    lame.exe		- kodek mp3 (http://www.free-codecs.com/download/Lame_Encoder.htm)
    sapi2wav.exe	- darmowy program wyko¿ystuj¹cy interfejs SAPI
                          do generowania próbek z lektorem (http://blog.hacker.dk/)
    sox.exe		- manipulacja plikami d¿wiêkowymi (http://sox.sourceforge.net/)
    wget.exe		- pobierania plków (http://www.gnu.org/software/wget/)

    *.dll		- potrzebne biblioteki

  katalog dat:
   testowy.smuxgen      - przyk³adowy plik tekstowy dla kursu w jêzyku niemieckim

 smuxgen.bat		- przyk³adowy skrypt startowy dla pliku testowy.smuxgen
 readme.txt		- plik który w³aœnie czytasz

4. Tworzenie nowego kursu.

  a. Zainstaluj program SuperMemo UX (http://www.supermemo.pl/)
  b. Pobierz aktualizacje
  c. Utwórz nowy kurs w programie SuperMemo (kurs->dodaj->utwórz)
     W katalogu "C:\Program Files\SuperMemo UX\Courses\" zostanie stworzony katalog z nowym kursem.
  d. Uwórz plik kurs.smuxgen ze s³owkami do zapamiêtania
  e. uruchom program smuxgen (zobacz smuxgen.bat)
  f. Po dodaniu nowych s³ówek, lub zmianie istniejêcyh uruchom jeszcze raz smuxgen.
     
5. Struktóra pliku smuxgen z kursem.

Pliki ze s³ówkami musz¹ byæ zapisane w kodowaniu UTF-8!

  W pierwszej linijce znajdujê siê tylko i wy³¹cznie œcie¿ka do pliku bazowego kursu(4c) np:
    C:\Program Files\SuperMemo UX\Courses\testowy\override\course.xml

  W drugiej linijce znajduje siê:
    NAZWA_LEKCJI parametry parametry parametry parametry

    NAZWA_LEKCJI - Nazwa lekcji pod która zostan¹ umieszczone s³ówka do zapamiêtania.
	           Jeden plik zawiera s³ówka do jednej lekcji.
		   Kilka ró¿nych plików mo¿e zawieraæ s³ówka do tego samego kursu.

    Parametry s³u¿a do modyfikowania zachowania programu SMUXGEN. 
    Parametry aktywuje siê poprzez wypisanie ich dok³adnej nazwy w drugiej linijce.
    Deaktywacja nastêpujê przez modyfikacje nazwy np. dodanie znaku "#":

	Force - wymusza wygenerowania wszystkich æwiczeñ ponownie (wraz z mp3 i obrazkami)
	Double- tworzy dodatkowo kurs alternatywny (jezyk obcy -> jezyk polski)
	Voice - Tworzy pliki mp3 z lektorem (skrypt createMp3.bat)
	Image - Tworzy pliki jpg (getGoogleHtml.bat i getImage.bat)
	Clean - Usuwa z kursu æwiczenia które zosta³y usuniête z pliku ze s³ówkami.
		(Mo¿e spowodowaæ problemy z dzia³aniem SuperMemo, opcja eksperymentalna)

  W kolejnych linijkach znajduj¹ siê s³ówka wed³ug schematu A:B|C
	A   - s³ówko w jêzyku polskim
	B,C - alternatywne odpowiedzi w jêzyku obcym

  Przyk³ady:
   	w koñcu(wreœcie):schließlich
	syrop na kaszel:der Hustensaft|der Hustensirup

6. Mp3 z lektorem.

SMUXGEN pozwala na generowania automatyczne mp3 z lektorem przy u¿yciu interfejsu SAPI.
(np. darmowe demo http://cepstral.com/). Po zainstalowaniu takiego silnika,
powinien on byæ widoczny do wyboru w panel sterowania->mowa.
Skrypt createMp3.bat jest odpowiedzialny za:
	- wygenerowanie odpowiedniego pliku wav dla danego s³ówka
	- ewentualny postrpocessing pliku wav (np. obciêcie, podg³óœnienie)
	- przekonwertowaniu wav na mp3
	
 
Oczywiœcie jakoœæ próbek z lektorem zale¿y tylko i wy³¹cznie od jakoœci u¿ytego silnika SAPI.


		


	

		

	 

  
    

  








		
 





 