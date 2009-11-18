@echo off
REM -----------------------------------
REM ---------createMP3.bat-------------
REM parametr %1 - nazwa pliku docelowego
REM parametr %2 - tekst do konwersji

REM -----------------------------------
REM ---------sapi2wav.exe---------------
REM parametr 1 - nazwa pliku docelowego
REM parametr 2 - numer enginu SAPI (nale¿y zmieniæ jeœli chcemy wybraæ inny engine SAPI)
REM parametr 3 - tekst do konwersji

REM -----------------------------------
REM -------------sox.exe---------------
REM trim 10 - obciêcie pierwszych 10 sekund
REM gain 15 - zwiêkszenie g³oœnoœci 

REM -----------------------------------
REM ------------lame.exe---------------
REM wav -> mp3

@echo on
sapi2wav.exe %1sox.wav 1 -t %2
sox.exe %1sox.wav %1.wav trim 10 gain 15
lame.exe %1.wav %1.mp3

@echo off
del %1.wav
del %1sox.wav
