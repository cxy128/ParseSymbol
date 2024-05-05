#include <Windows.h>
#include <iostream>
#include <dbghelp.h>
#include "util.h"
#include "symbol.h"

#pragma comment(lib,"Dbghelp.lib")

int main() {

	// "ntoskrnl.exe" "win32k.sys"
	char SystemDirectoryModuleName[] = "ntoskrnl.exe";

	if (!InitSystemRoutineAddress()) {
		__debugbreak();
		MessageBoxA(nullptr, "InitSystemRoutineAddress failed", "E", MB_OK);
	}

	auto ImageBase = GetModuleBaseAddress(SystemDirectoryModuleName);
	if (!ImageBase) {
		__debugbreak();
		MessageBoxA(nullptr, "GetModuleBaseAddress failed", "E", MB_OK);
	}

	auto Process = GetCurrentProcess();
	if (!InitializeSymbol(Process)) {
		__debugbreak();
		MessageBoxA(nullptr, "InitializeSymbol failed", "E", MB_OK);
	}

	auto ModuleBaseAddress = LoadSymbolTable(Process, SystemDirectoryModuleName, ImageBase);
	if (!ModuleBaseAddress) {
		__debugbreak();
		MessageBoxA(nullptr, "LoadSymbolTable failed", "E", MB_OK);
	}

	if (!SymEnumSymbols(Process, ModuleBaseAddress, nullptr, PsymEnumeratesymbolsCallback, nullptr)) {
		__debugbreak();
		MessageBoxA(nullptr, "SymEnumSymbols failed", "E", MB_OK);
	}

	return 0;
}