#include <iostream>
#include <fstream>
#include <cassert>
#include <windows.h>
#include <shellapi.h>
#include <theypsilon/error.h>
#include <thread>

LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs)
{
	int retval;
	retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, NULL, 0);
	if (!SUCCEEDED(retval))
		return NULL;

	LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
	if (lpWideCharStr == NULL)
		return NULL;

	retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
	if (!SUCCEEDED(retval))
	{
		free(lpWideCharStr);
		return NULL;
	}

	int numArgs;
	LPWSTR* args;
	args = CommandLineToArgvW(lpWideCharStr, &numArgs);
	free(lpWideCharStr);
	if (args == NULL)
		return NULL;

	int storage = numArgs * sizeof(LPSTR);
	for (int i = 0; i < numArgs; ++i)
	{
		BOOL lpUsedDefaultChar = FALSE;
		retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, NULL, 0, NULL, &lpUsedDefaultChar);
		if (!SUCCEEDED(retval))
		{
			LocalFree(args);
			return NULL;
		}

		storage += retval;
	}

	LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
	if (result == NULL)
	{
		LocalFree(args);
		return NULL;
	}

	int bufLen = storage - numArgs * sizeof(LPSTR);
	LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
	for (int i = 0; i < numArgs; ++i)
	{
		assert(bufLen > 0);
		BOOL lpUsedDefaultChar = FALSE;
		retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, NULL, &lpUsedDefaultChar);
		if (!SUCCEEDED(retval))
		{
			LocalFree(result);
			LocalFree(args);
			return NULL;
		}

		result[i] = buffer;
		buffer += retval;
		bufLen -= retval;
	}

	LocalFree(args);

	*pNumArgs = numArgs;
	return result;
}

ty::error program(int argc, char* argv[]);


int CALLBACK WinMain(
	HINSTANCE   hInstance,
	HINSTANCE   hPrevInstance,
	LPSTR       lpCmdLine,
	int         nCmdShow
) {
	std::ofstream out;
	try {
		std::string logfile = std::string(PROJECT_BINARY_NAME) + "-" + std::string(PROJECT_VERSION) + ".log";
		out.open(logfile.c_str(), std::ios::out | std::ios::trunc);
		std::cout.rdbuf(out.rdbuf());
		std::cerr.rdbuf(out.rdbuf());
	}
	catch (...) {}
	int argc;
	LPSTR * argv = CommandLineToArgvA(GetCommandLineA(), &argc);
	auto err = program(argc, argv);
	LocalFree(argv);
	if (err) {
		std::cerr << "[ERROR] " << err.msg;
		MessageBoxA(
			NULL, 
			"Some unexpected error happened.\nMake sure you have installed the graphic drivers correctly.\nIf the problem persists, contact the author at:\ntheypsilon@gmail.com",
			"Message", 
			MB_OK | MB_ICONERROR
		);
		return -1;
	}
	return 0;
}