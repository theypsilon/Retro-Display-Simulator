#!/usr/bin/env bash

set -euo pipefail

echo "#ifndef RESOURCES_BINARIES" > src/resources_binaries.h
echo "#define RESOURCES_BINARIES" >> src/resources_binaries.h

cd resources/shaders/

function multiline() {
	echo "const char* ${1} = R\"SHADER(" >> ../../src/resources_binaries.h
	cat ${2} >> ../../src/resources_binaries.h
	echo ")SHADER\";" >> ../../src/resources_binaries.h
}

multiline voxel_vs voxel.vs
multiline voxel_fs voxel.fs
multiline info_panel_vs info_panel.vs
multiline info_panel_fs info_panel.fs

cd ../textures/
for filename in * ; do
    xxd -i ${filename} >> ../../src/resources_binaries.h
done
echo "#endif" >> ../../src/resources_binaries.h
