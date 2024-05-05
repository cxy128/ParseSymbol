#pragma once

bool InitializeSymbol(HANDLE Process);

unsigned __int64 LoadSymbolTable(HANDLE Process, char* ModuleName, unsigned __int64 ImageBase);

BOOL PsymEnumeratesymbolsCallback(SYMBOL_INFO* SymInfo, ULONG SymbolSize, PVOID UserContext);