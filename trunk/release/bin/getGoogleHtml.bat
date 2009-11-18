@echo off
REM -----------------------------------
REM -------getGoogleHtml.bat-----------
REM wyszukuje obrazki na stronie google images
REM parametr %1 - wyszukiwana fraza
REM parametr %2 - plik docelowy
REM
REM Poni¿sze operacja na zmiennej tmp, maj¹ na celu usuniêcie cudzys³owia z przekazywanego parametru

@echo off
set tmp=%1
set tmp=%tmp:"=%

@echo on
wget -U "Mozilla/5.0" -T 60 -t 5 -O %2 "http://images.google.pl/images?q=%tmp%+filetype:jpg&imgsz=small|medium|large|xlarge"


