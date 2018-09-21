@echo off

SET "version=0.1.0"
SET "zipname=rvd-%version%.zip"

if not exist "build" (
	mkdir build
)
if not exist "releases" (
	mkdir releases
)
if exist "build\release" (
	RD /S /Q build\release
)
if exist "releases\%zipname%" (
	DEL "releases\%zipname%"
)
mkdir build\release
mkdir build\release\resources

xcopy retro-voxel-display.exe build\release\ > nul
xcopy resources build\release\resources\ /S /E > nul

rcedit build\release\retro-voxel-display.exe^
    --set-version-string "Comments" "Ping the creator at theypsilon@gmail.com"^
	--set-version-string "ProductName" "Retro Voxel Display"^
	--set-version-string "LegalCopyright" "GNU GPLv3"^
	--set-version-string "CompanyName" "José Manuel Barroso Galindo"^
	--set-file-version %version%^
	--set-product-version %version%^
	--set-icon cubes.ico

powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('build\release', 'releases\rvd-%version%.zip'); }" > nul
echo Created file %zipname% on releases folder.
