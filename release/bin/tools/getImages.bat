:@echo off
:REM -----------------------------------
:REM -------getGoogleHtml.bat-----------
:REM wyszukuje obrazki na stronie google images
:REM parametr %1 - search url
:REM parametr %2 - output file
:REM

set MYDIR=%~dp0 
set tmp=%~1
"%MYDIR%\\wget.exe" --no-check-certificate -U "Mozilla/5.0 Firefox/25.0" -T 60 -t 5 -O %2 %1


