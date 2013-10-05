:@echo off
REM -----------------------------------
REM ---------createMP3.bat-------------
REM parametr %1 - nazwa pliku docelowego
REM parametr %2 - tekst do konwersji
REM parametr %3 - numer silnika d�wi�ku
REM parametr %4 - trim
REM parametr %5 - gain

REM -----------------------------------
REM ---------sapi2wav.exe---------------
REM parametr 1 - nazwa pliku docelowego
REM parametr 2 - numer enginu SAPI (nale�y zmieni� je�li chcemy wybra� inny engine SAPI)
REM parametr 3 - tekst do konwersji

REM -----------------------------------
REM -------------sox.exe---------------
REM trim 10 - obci�cie pierwszych 10 sekund
REM gain 15 - zwi�kszenie g�o�no�ci 

REM -----------------------------------
REM ------------lame.exe---------------
REM wav -> mp3

set MYDIR=%~dp0
set F=%~pn1
del /F /Q %1
"%MYDIR%\\sapi2wav.exe" %F%sox.wav %3 -t %2
"%MYDIR%\\sox.exe" %F%sox.wav %F%.wav trim %4 gain %5
"%MYDIR%\\lame.exe" %F%.wav %1
exit 1

