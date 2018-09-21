@echo off

SET "version=0.1.0"
SET "filename=rvd-%version%.exe"

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

xcopy retro-voxel-display.exe build\release\ > nul

rcedit build\release\retro-voxel-display.exe^
    --set-version-string "Comments" "Ping the creator at theypsilon@gmail.com"^
	--set-version-string "ProductName" "Retro Voxel Display"^
	--set-version-string "LegalCopyright" "GNU GPLv3"^
	--set-version-string "CompanyName" "José Manuel Barroso Galindo"^
	--set-file-version %version%^
	--set-product-version %version%^
	--set-icon cubes.ico

copy build\release\retro-voxel-display.exe releases\%filename% > nuls

echo Created file %filename% on releases folder.
