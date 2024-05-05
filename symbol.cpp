#include <Windows.h>
#include <iostream>
#include <dbghelp.h>
#include "symbol.h"

bool InitializeSymbol(HANDLE Process) {

	char CurrentDirectory[MAX_PATH] = "";
	if (!GetCurrentDirectoryA(MAX_PATH, CurrentDirectory)) {
		return false;
	}

	strcat_s(CurrentDirectory, "\\symbols");

	if (!CreateDirectoryA(CurrentDirectory, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
		return false;
	}

	char FileName[MAX_PATH] = "";
	strncpy_s(FileName, CurrentDirectory, MAX_PATH);
	strcat_s(FileName, "\\symsrv.yes");

	auto FileHandle = CreateFileA(FileName, FILE_ALL_ACCESS, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE) {
		return false;
	}

	char SymbolPath[MAX_PATH] = "";
	auto SymbolsUrl = "https://msdl.microsoft.com/download/symbols";

	sprintf_s(SymbolPath, MAX_PATH, "SRV*%s*%s", CurrentDirectory, SymbolsUrl);

	SymSetOptions(SYMOPT_CASE_INSENSITIVE | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);

	if (!SymInitialize(Process, SymbolPath, TRUE)) {
		CloseHandle(FileHandle);
		return false;
	}

	if (!SymSetSearchPath(Process, SymbolPath)) {
		CloseHandle(FileHandle);
		return false;
	}

	CloseHandle(FileHandle);
	return true;
}

unsigned __int64 LoadSymbolTable(HANDLE Process, char* ModuleName, unsigned __int64 ImageBase) {

	char SymbolFilePath[MAX_PATH] = "";
	if (!GetSystemDirectoryA(SymbolFilePath, MAX_PATH)) {
		return 0;
	}

	strcat_s(SymbolFilePath, "\\");
	strcat_s(SymbolFilePath, ModuleName);

	char SymbolFile[MAX_PATH] = "";
	char DbgFile[MAX_PATH] = "";
	if (!SymGetSymbolFile(Process, NULL, SymbolFilePath, sfPdb, SymbolFile, MAX_PATH, DbgFile, MAX_PATH)) {
		// GetLastError() == 2 请在.exe文件路径下添加 dbghelp.dll symsrv.dll
		return 0;
	}

	HANDLE FileHandle = CreateFileA(SymbolFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE) {
		return 0;
	}

	LARGE_INTEGER FileSize = {};
	if (!GetFileSizeEx(FileHandle, &FileSize)) {
		CloseHandle(FileHandle);
		return 0;
	}

	auto ModuleBaseAddress = SymLoadModuleEx(Process, FileHandle, SymbolFilePath, nullptr, ImageBase, FileSize.QuadPart, nullptr, 0);
	if (!ModuleBaseAddress) {
		CloseHandle(FileHandle);
		return 0;
	}

	CloseHandle(FileHandle);
	return ModuleBaseAddress;
}

BOOL PsymEnumeratesymbolsCallback(SYMBOL_INFO* SymInfo, ULONG SymbolSize, PVOID UserContext) {

	printf_s("FunctionName: %s \t FunctionAddress: %llx \n", SymInfo->Name, SymInfo->Address);

	return TRUE;
}
