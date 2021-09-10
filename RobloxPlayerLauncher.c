#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "console_spoof.h"

// this code is mostly stolen from https://github.com/yafyz/roblox_player_launcher/blob/master/main.c
int main(int argc, char* argv[]) {
    // argv[1] parser
    char* launchstr = argv[1];
    char ch;

    int larg_len = 0;
    int larg_s = 0;
    int v_split_off = 0;
    int v_len;
    int urlenc_count = 0;

    char* authticket;
    char* placelauncher;

    for (int i = 0; i < strlen(launchstr); i++) {
        ch = launchstr[i];
        larg_len++;

        if (ch == '+') {
            if (strncmp("gameinfo", launchstr+larg_s, v_split_off-1) == 0) {
                v_len = larg_len-v_split_off-1;
                authticket = (char*) malloc(v_len+1);
                memcpy(authticket, launchstr+larg_s+v_split_off, v_len);
                authticket[v_len] = 0;
            } else if (strncmp("placelauncherurl", launchstr+larg_s, v_split_off-1) == 0) {
                v_len = larg_len-v_split_off-1;
                placelauncher = (char*) malloc(v_len-urlenc_count*2+1);
                int len = decode(launchstr+larg_s+v_split_off, placelauncher, v_len);
                placelauncher[len] = 0;
            }
            larg_s = i+1;
            larg_len = 0;
            v_split_off = 0;
            urlenc_count = 0;
        } else if (ch == ':') {
            v_split_off = larg_len;
        } else if (ch == '%') {
            urlenc_count++;
        }
    }

    // Roblox path parser
    char lpRbxPath[FILENAME_MAX];
    int offLastSlash = 0;

    GetModuleFileNameA(NULL, lpRbxPath, FILENAME_MAX);
    for (int i = 0; i < strlen(lpRbxPath); i++)
        if (lpRbxPath[i] == '\\' || lpRbxPath[i] == '/')
            offLastSlash = i;
    lpRbxPath[offLastSlash+1] = 0;
    sprintf(lpRbxPath, "%sRobloxPlayerBeta.exe", lpRbxPath);

    // Arguments string
    int szArgs = sizeof("--play -t  -j  --rloc en_us --gloc en_us") + strlen(authticket) + strlen(placelauncher) + 1;
    char* lpArgs = malloc(szArgs);
    memset(lpArgs, 0, szArgs);
    sprintf_s(lpArgs, szArgs, "--play -t %s -j %s --rloc en_us --gloc en_us", authticket, placelauncher);

    // Launching roblox
    PROCESS_INFORMATION procInfo;
    STARTUPINFOA startInfo;

    memset(&procInfo,0,sizeof(procInfo));
    memset(&startInfo,0,sizeof(startInfo));

    if (!CreateProcessA(lpRbxPath, lpArgs, NULL, NULL, FALSE, 0, NULL, NULL, &startInfo, &procInfo)) {
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
    ConsoleSpoof(hProc, "RobloxPlayerBeta.exe");
    Sleep(3000);

    return 0;
}

inline int ishex(int x) { // not feeling like writing a url decode rn https://www.rosettacode.org/wiki/URL_decoding#C
	return (x >= '0' && x <= '9') ||
		   (x >= 'a' && x <= 'f') ||
		   (x >= 'A' && x <= 'F');
}

int decode(char *s, char *dec, int len) {
	char* o;
	const char* end = s + len;
	int c;

	for (o = dec; s <= end; o++) {
		c = *s++;
		if (c == '+')
            c = ' ';
		else if (c == '%' && (!ishex(*s++) || !ishex(*s++) || !sscanf(s - 2, "%2x", &c)))
			return -1;
		if (dec)
            *o = c;
	}
	return o - dec;
}