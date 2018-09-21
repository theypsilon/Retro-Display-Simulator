#include <theypsilon/windows.h>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]);

int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow
) {
	std::ofstream out;
	try {
		out.open("retro-voxel-display.log", std::ios::out | std::ios::trunc);
		std::cout.rdbuf(out.rdbuf());
		std::cerr.rdbuf(out.rdbuf());
	}
	catch (...) {}
	int argc;
	LPSTR * argv = CommandLineToArgvA(GetCommandLineA(), &argc);
	auto result = main(argc, argv);
	LocalFree(argv);
	return result;
}