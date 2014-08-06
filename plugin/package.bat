::@echo off
SET ROOT=%1
SET TYPE=%2
SET DESTINATION=%ROOT%package\%TYPE%\
SET NAME=%3

echo Copying files...
echo Type: %TYPE%, root: %ROOT%, destination: %DESTINATION%
mkdir %DESTINATION% > nul 2> nul
copy /y %ROOT%%TYPE%\%NAME%.dll %DESTINATION% 1>nul
copy /y %ROOT%%TYPE%\%NAME%.pdb %DESTINATION% 1>nul
copy /y %ROOT%p4-far-plugin_en.lng %DESTINATION%\%NAME%_en.lng 1>nul
copy /y %ROOT%p4-far-plugin_en.hlf %DESTINATION%\%NAME%_en.hlf 1>nul
