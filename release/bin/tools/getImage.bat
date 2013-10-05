
REM -----------------------------------
REM ------------getImage.bat-----------
REM Pobiera plik na dysk
REM parametr %1 - URL pliku
REM parametr %2 - plik docelowy

del /F /Q %2
set MYDIR=%~dp0
"%MYDIR%\\wget.exe" -U "Mozilla/5.0"  -T 60 -t 5 -O %2 %1



 


 


