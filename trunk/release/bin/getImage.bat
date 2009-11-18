@echo off
REM -----------------------------------
REM ------------getImage.bat-----------
REM Pobiera plik na dysk
REM parametr %1 - URL pliku
REM parametr %2 - plik docelowy
REM
REM Poni¿sze operacja na zmiennej tmp, maj¹ na celu usuniêcie cudzys³owia i ostatniej spacji z przekazywanego parametru
REM Spacja dodawana jest celowo, ¿eby argumenty zosta³y przekazane w cudzys³owie.
REM Te zabiegi wynikaj¹ ze sposobu w jaki linia poleceñ interpretuje parametry 

del img
set tmp1=%1
set tmp1=%tmp1:"=%
set tmp1=%tmp1:~0,-1%

set tmp2=%2
set tmp2=%tmp2:"=%
set tmp2=%tmp2:~0,-1%

echo arg1=[%tmp1%]
echo arg2=[%tmp2%]

@echo on
wget -U "Mozilla/5.0"  -T 60 -t 5 -O img "%tmp1%" 
move img "%tmp2%.jpg"



 


