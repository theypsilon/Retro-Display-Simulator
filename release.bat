@echo off

SET /p version=<configuration/version.var
SET /p binary_name=<configuration/binary_name.var
SET /p project_name=<configuration/project_name.var
SET "filename=%binary_name%-%version%.exe"

if not exist "build" (
	mkdir build
)
if not exist "releases" (
	mkdir releases
)
if exist "build\release" (
	RD /S /Q build\release
)
if exist "releases\%filename%" (
	DEL "releases\%filename%"
)
mkdir build\release

xcopy %binary_name%.exe build\release\ > nul || goto :error

rcedit build\release\%binary_name%.exe^
    --set-version-string "Comments" "Ping the creator at theypsilon@gmail.com"^
	--set-version-string "ProductName" "%project_name%"^
	--set-version-string "LegalCopyright" "GNU GPLv3"^
	--set-version-string "CompanyName" "Jos� Manuel Barroso Galindo"^
	--set-file-version %version%^
	--set-product-version %version%^
	--set-icon icon72.ico || goto :error

copy build\release\%binary_name%.exe releases\%filename% > nul || goto :error

echo Created file %filename% on releases folder.
goto :EOF

:error
echo "Failed with error #%errorlevel%".
exit /b %errorlevel%
