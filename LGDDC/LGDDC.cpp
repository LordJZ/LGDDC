
#include <Windows.h>
#include <new>
#include "../LGMonitorDDCCISDK/LGMonitorDDCCISDK.h"

void usage() {
	MessageBox(NULL, TEXT("Usage: LGDDC <property> <value>"), TEXT("Usage"), 0);
}

void freemon(CMonitorDDCCISDK* mon, int monitorId) {
	mon->UnInitializeDDCCISDK(monitorId);
	mon->~CMonitorDDCCISDK();
}

CMonitorDDCCISDK* trymon(void* mem, int monitorId) {
	CMonitorDDCCISDK* mon = new (mem) CMonitorDDCCISDK;

	if (mon->InitializeDDCCIStack(++monitorId)) {
		freemon(mon, monitorId);
		return nullptr;
	}

	return mon;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
) {
	int nArgs;
	LPWSTR* p = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (nArgs < 3) {
		usage();
		return 1;
	}

	int prop = _wtoi(p[1]);
	int val = _wtoi(p[2]);
	if (!prop || !val) {
		usage();
		return 1;
	}

	void* mem = malloc(100000);

	HKEY key;
	RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\LGDDC"), 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL);

	DWORD monitorId = 0;
	DWORD sz = sizeof(monitorId);
	RegGetValue(key, NULL, TEXT("MonitorId"), RRF_RT_REG_DWORD, NULL, &monitorId, &sz);

	CMonitorDDCCISDK* mon = nullptr;
	if (!(mon = trymon(mem, monitorId))) {
		monitorId = 0;

		while (!(mon = trymon(mem, monitorId))) {
			if (++monitorId >= 6)
				return 1;
		}

		RegSetValueEx(key, TEXT("MonitorId"), 0, REG_DWORD, (const BYTE*)(void*)&monitorId, sizeof(monitorId));
	}

	RegCloseKey(key);

	mon->SetPropertyWithoutOpcodeVerification((T_E_OPCODE)prop, val);

	freemon(mon, monitorId);

	return 0;
}
