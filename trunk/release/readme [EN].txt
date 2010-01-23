--------------------------------------------------------------------------------
			SMUXGEN - SuperMemo UX Generator
--------------------------------------------------------------------------------
Version 2.1 (Tested with SuperMemo UX 1.4.3.2)
Author	Marcin Krystianc (marcin.krystianc@gmail.com)
OS	Windows
License GPL
URL 	http://code.google.com/p/smuxgen/
--------------------------------------------------------------------------------



1. What is SMUXGEN ?

SMUXGEN is totally free software to help creating custom flash cards for SuperMemo UX.
Course is generated automatically basing on Your entries    in text file.
In addition it is possible for SMUXGEN to get corresponding pictures through 
Internet connection and create mp3 with sample pronunciation.

2. SuperMemo method (http://www.supermemo.eu/supermemo_method).

3. Installation.

SMUXGEN doesn't require installation, just need to be unpacked.
All components are included, but for mp3 You need to install SAPI ("Microsoft Speach API") engine.

Archive contain:
  bin directory:
    smuxgen.exe		- main executable
    createMp3.bat 	- script responsible for mp3 
    getGoogleHtml.bat 	- script responsible for getting google images result
    getImage.bat	- script for downloading images

    lame.exe		- mp3 codec (http://www.free-codecs.com/download/Lame_Encoder.htm)
    sapi2wav.exe	- free tool interacing SAPI technology (http://blog.hacker.dk/)
    sox.exe		- to manipulate wav files (http://sox.sourceforge.net/)
    wget.exe		- to get files from Internet (http://www.gnu.org/software/wget/)

    *.dll		- other libraries

  dat direcotory:
   testowy.smuxgen      - example file for German-Polish course

 smuxgen.bat		- start SMUXGEN
 smuxgen (batch).bat	- example how to use SMUXGEN in batch mode

 readme [PL].txt	- polish readme
 readme [EN].txt	- english readme

4. To create new course.
  a. Install SuperMemo UX (http://www.supermemo.pl/)
  b. Get all updates (WITHOUT UPDATES SMUXGEN WILL NOT WORK, tested with version 1.4.3.2)
  c. Create new course (course->subscribe->create)
     in directory "C:\Program Files\SuperMemo UX\Courses\" new direcotry with course name will be created .
  d. Start SMUXGEN (smuxgen.bat)
  e. See example file and create Your own file
  f. Set correct path to database file and select correct course name (see options)
  g. Save it and press GENERATE button (See conole window for diagnostics).
  h. After any change just generate course again 
     
5. Options.
  
  Database file: - path to  SuperMemo UX dataabse file name ("C:/Documents and Settings/????/Application Data/SuperMemo World/SuperMemo UX/?????/Repetitions.dat"
  Course	     - Course name to which current lesson will be added
  Regenerate 	 - create again all flash cards
  Alternate 	 - create mirrored course
  Images	- add pictures
  Lector	- add mp3 with sample  pronunciation
  
 
  Word List:
    In following lines enter A:B|C
	A   - word in native language
	B,C - words in foreign language

  Example:
   	w koñcu(wreszcie):schließlich
	syrop na kaszel:der Hustensaft|der Hustensirup

6. Mp3

SMUXGEN is able to add automatically sample pronunciation using SAPI interface.
You need to install such engine separately (like http://cepstral.com/) 
After installing it it should be visible in control panel->Speech

Script createMp3.bat is responsible for:
	- generating wav file for given text
	- post-processing (trim, gain)
	- converting to mp3

Options:
  Voice index	- index of SAPI engine
  Gain			- gain
  Trim			- trim in seconds
   
With test button You can check this settings (text from edit box will be read).
 
7. Picture Browser

This is function which allows to change pictures for already generated course.
New pictures are downloaded from Internet. When default results are insufficient, it is possible to use custom key words.
To change unwanted picture just use "drag and drop" technique.

8. Changelog

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
 