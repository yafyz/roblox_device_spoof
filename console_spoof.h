#include <windows.h>
#include <psapi.h>
#include <stdio.h>

const char* nwUserAgent = "Roblox/XboxOne";
const char* ogUserAgent = "Roblox/WinInet";

typedef LONG (NTAPI *t_NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG (NTAPI *t_NtResumeProcess)(IN HANDLE ProcessHandle);

t_NtSuspendProcess NtSuspendProcess;
t_NtResumeProcess NtResumeProcess;

uintptr_t getModuleBaseAddress(HANDLE hProc, const char* mToFind) {
    HMODULE hMods[1024];
    DWORD dwBytesNeeded;
    char lpModName[MAX_PATH];

    if (!EnumProcessModules(hProc, hMods, sizeof(hMods), &dwBytesNeeded)) {
        printf("epic EnumProcessModules fail\n");
        getchar();
        exit(1);
    }

    for (int i = 0; i < (dwBytesNeeded / sizeof(HMODULE)); i++) {
        if (GetModuleBaseNameA(hProc, hMods[i], lpModName, sizeof(lpModName)) && (strcmp(mToFind, lpModName) == 0)) {
            return (uintptr_t)hMods[i];
        }
    }

    return (uintptr_t)NULL;
}

void ConsoleSpoof(HANDLE hProc, const char* lpModuleName) {
    char lpBuff[15];
    uintptr_t baseAddr;
    uintptr_t currAddr;
    uintptr_t nextRegionStart = 0;
    MEMORY_BASIC_INFORMATION mbi;
    BOOL didFind = FALSE;

    NtSuspendProcess = (t_NtSuspendProcess)GetProcAddress(GetModuleHandleA("ntdll"), "NtSuspendProcess");
    NtResumeProcess = (t_NtResumeProcess)GetProcAddress(GetModuleHandleA("ntdll"), "NtResumeProcess");

    memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));
    memset(lpBuff, 0, sizeof(lpBuff));
    NtSuspendProcess(hProc);


    baseAddr = getModuleBaseAddress(hProc, lpModuleName);
    currAddr = baseAddr;
    printf("Base Address: 0x%0x\n", currAddr);

    printf("\nScanning\n");
    while (currAddr++ < 0x7FFFFFFF) {
        if (currAddr >= nextRegionStart) {
            if (!VirtualQueryEx(hProc, (LPCVOID)currAddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION))) {
                printf("\rVirtualQueryEx failed on address 0x%0.8x, err: %d\n", currAddr, GetLastError());
                continue;
            }

            if ((uintptr_t)mbi.AllocationBase != baseAddr)
                break;

            if (mbi.Protect != PAGE_READWRITE) {
                printf("\rSkipping region, Base: 0x%0.8x Size: 0x%0.8x\n", (uintptr_t)mbi.BaseAddress, mbi.RegionSize);
                currAddr = (uintptr_t)mbi.BaseAddress + mbi.RegionSize + 1;
            } else {
                printf("Region Base: 0x%0.8x Size: 0x%0.8x, Protect: 0x%0.8x\n", (uintptr_t)mbi.BaseAddress, mbi.RegionSize, mbi.Protect);
                nextRegionStart = (uintptr_t)mbi.BaseAddress + mbi.RegionSize + 1;
            }
        }

        if (ReadProcessMemory(hProc, (LPCVOID)currAddr, lpBuff, sizeof(lpBuff)-1, NULL) == FALSE) {
            printf("\rread fail");
            continue;
        }

        if ((currAddr % 0x10000) == 0)
            printf("\r0x%0.8x", currAddr);

        if (strcmp(ogUserAgent, lpBuff) == 0) {
            printf("\r\n");
            didFind = TRUE;
            break;
        }
    }

    printf("\ndidFind: %d\n", didFind);
    if (didFind) {
        printf("address: 0x%0x\nbuff: %s\n", currAddr, lpBuff);
        SIZE_T bytesWritten = 0;
        if (!WriteProcessMemory(hProc, (LPVOID)currAddr, (LPCVOID)nwUserAgent, strlen(nwUserAgent), &bytesWritten)) {
            printf("Failed to overwrite UserAgent string\n");
        } else {
            printf("UserAgent string overwritten successfully, bytes written: %d\n", bytesWritten);
        }
    }

    NtResumeProcess(hProc);
    CloseHandle(hProc);
}
