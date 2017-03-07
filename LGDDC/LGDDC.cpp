
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
	CMonitorDDCCISDK* mon = nullptr;
	int monitorId = 0;
	do
	{
		if (mon) {
			freemon(mon, monitorId);
		}

		mon = new (mem) CMonitorDDCCISDK;

	} while (mon->InitializeDDCCIStack(++monitorId) && monitorId < 6);

	mon->SetPropertyWithoutOpcodeVerification((T_E_OPCODE)prop, val);

	freemon(mon, monitorId);

	return 0;
}
