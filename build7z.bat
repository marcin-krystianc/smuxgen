set name=smuxgen v2.23
xcopy /D /Y "%~dp0\build-smuxgen-qt_4_8-Release\release\smuxgen.exe" "%~dp0\release\bin\"
rmdir "%name%" /S /Q
mkdir "%name%" 
xcopy /D /Y /S "%~dp0\release" "%~dp0\\%name%"
"C:\Program Files (x86)\7-Zip\7z.exe" a -t7z -mx9 "%name%.7z" "%name%\*"
pause