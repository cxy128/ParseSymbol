#pragma once

#define PAGE_SHIFT 12L

#define PAGE_SIZE 0x1000

#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)

enum class SYSTEM_INFORMATION_CLASS :unsigned __int32 {

	SystemModuleInformation = 0xb
};

struct RTL_PROCESS_MODULE_INFORMATION {

	PVOID  	Section;
	PVOID 	MappedBase;
	PVOID 	ImageBase;
	ULONG 	ImageSize;
	ULONG 	Flags;
	USHORT 	LoadOrderIndex;
	USHORT 	InitOrderIndex;
	USHORT 	LoadCount;
	USHORT 	OffsetToFileName;
	CHAR 	FullPathName[256];
};

struct RTL_PROCESS_MODULES {

	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
};

using fnZwQuerySystemInformation = NTSTATUS(*)(SYSTEM_INFORMATION_CLASS SystemInfoClass, PVOID SystemInfoBuffer, ULONG SystemInfoBufferSize, PULONG BytesReturned);
using fnZwAllocateVirtualMemory = NTSTATUS(*)(HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);

inline fnZwQuerySystemInformation fZwQuerySystemInformation;
inline fnZwAllocateVirtualMemory fZwAllocateVirtualMemory;