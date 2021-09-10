#include <windows.h>
#include "console_spoof.h"

const char* lpRobloxName = "ogRobloxPlayerBeta.exe";

int main(int argc, char* argv[]) {
    PROCESS_INFORMATION procInfo;
    STARTUPINFOA startInfo;

    memset(&procInfo,0,sizeof(procInfo));
    memset(&startInfo,0,sizeof(startInfo));

    char lpRbxPath[FILENAME_MAX];
    int offLastSlash = 0;

    GetModuleFileNameA(NULL, lpRbxPath, FILENAME_MAX);
    for (int i = 0; i < strlen(lpRbxPath); i++)
        if (lpRbxPath[i] == '\\' || lpRbxPath[i] == '/')
            offLastSlash = i;
    lpRbxPath[offLastSlash+1] = 0;
    sprintf(lpRbxPath, "%s%s", lpRbxPath, lpRobloxName);

    if (!CreateProcessA(lpRbxPath, GetCommandLineA(), NULL, NULL, FALSE, 0, NULL, NULL, &startInfo, &procInfo)) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        getchar();
        exit(1);
    }

    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procInfo.dwProcessId);
    if (hProc == NULL) {
        printf("fucky wucky no proc handle\n");
        getchar();
        exit(1);
    }

    printf("Handle: 0x%0x\n", (UINT)hProc);
    Sleep(1500);
    ConsoleSpoof(hProc, lpRobloxName);
    Sleep(3000);
    return 0;
}