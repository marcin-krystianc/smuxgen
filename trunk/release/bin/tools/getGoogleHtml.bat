@echo off
REM -----------------------------------
REM -------getGoogleHtml.bat-----------
REM wyszukuje obrazki na stronie google images
REM parametr %1 - wyszukiwana fraza
REM parametr %2 - plik docelowy
REM
REM %~I         - expands %I removing any surrounding quotes (")

set MYDIR=%~dp0
set tmp=%~1
"%MYDIR%\\wget.exe" -U "Mozilla/5.0" -T 60 -t 5 -O %2 "http://images.google.pl/images?q=%tmp%+filetype:jpg&safe=active&imgsz=small|medium|large|xlarge"


