
#include <Windows.h>
#include <new>
#include "../LGMonitorDDCCISDK/LGMonitorDDCCISDK.h"

void usage() {
	MessageBox(NULL, TEXT("Usage: LGDDC <property> <value>"), TEXT("Usage"), 0);
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

	CMonitorDDCCISDK* mon = new (malloc(100000)) CMonitorDDCCISDK;
	mon->InitializeDDCCIStack(3);

	mon->SetPropertyWithoutOpcodeVerification((T_E_OPCODE)prop, val);

	mon->UnInitializeDDCCISDK(3);
	mon->~CMonitorDDCCISDK();
}
