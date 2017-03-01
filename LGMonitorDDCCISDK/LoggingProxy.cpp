
#include "Windows.h"
#include <stdio.h>
#include <time.h>

#define LGDDCCISDK_BUILD
#include "LGMonitorDDCCISDK.h"

static bool Initialized = false;

//typedef CMonitorDDCCISDK*(CMonitorDDCCISDK::*TCtor)();
using TCtor = CMonitorDDCCISDK*(CMonitorDDCCISDK::*)();
using TDtor = CMonitorDDCCISDK*(CMonitorDDCCISDK::*)();
using TInit = decltype(&CMonitorDDCCISDK::InitializeDDCCIStack);
using TUnInit = decltype(&CMonitorDDCCISDK::UnInitializeDDCCISDK);
using TGet = decltype(&CMonitorDDCCISDK::GetPropertyWithoutOpcodeVerification);
using TSet = decltype(&CMonitorDDCCISDK::SetPropertyWithoutOpcodeVerification);

static TCtor _ctor = nullptr;
static TDtor _dtor = nullptr;
static TInit _init = nullptr;
static TUnInit _uninit = nullptr;
static TGet _get = nullptr;
static TSet _set = nullptr;

template<typename T>
static void Check(T ptr, LPCTSTR what) {
	if (!ptr) {
		MessageBox(NULL, what, TEXT("Error"), 0);
		exit(1);
	}
}

static void EnsureInitialized() {
	if (Initialized)
		return;

	HMODULE lib = LoadLibrary(TEXT("C:\\Windows\\System32\\LGMonitorDDCCISDK.dll"));
	if (!lib) {
		lib = LoadLibrary(TEXT("C:\\Windows\\SysWOW64\\LGMonitorDDCCISDK.dll"));
	}

	Check(lib, TEXT("DLL"));

	*(void**)&_ctor = (void*)GetProcAddress(lib, "??0CMonitorDDCCISDK@@QAE@XZ");
	*(void**)&_dtor = (void*)GetProcAddress(lib, "??1CMonitorDDCCISDK@@QAE@XZ");
	*(void**)&_init = (void*)GetProcAddress(lib, "?InitializeDDCCIStack@CMonitorDDCCISDK@@QAE?AW4_T_E_MONITOR_ERROR@@H@Z");
	*(void**)&_uninit = (void*)GetProcAddress(lib, "?UnInitializeDDCCISDK@CMonitorDDCCISDK@@QAE?AW4_T_E_MONITOR_ERROR@@H@Z");
	*(void**)&_get = (void*)GetProcAddress(lib, "?GetPropertyWithoutOpcodeVerification@CMonitorDDCCISDK@@QAE?AW4_T_E_MONITOR_ERROR@@W4T_E_OPCODE@@AAG1@Z");
	*(void**)&_set = (void*)GetProcAddress(lib, "?SetPropertyWithoutOpcodeVerification@CMonitorDDCCISDK@@QAE?AW4_T_E_MONITOR_ERROR@@W4T_E_OPCODE@@G@Z");

	Check(_ctor, TEXT("ctor"));
	Check(_dtor, TEXT("dtor"));
	Check(_init, TEXT("init"));
	Check(_uninit, TEXT("uninit"));
	Check(_get, TEXT("get"));
	Check(_set, TEXT("set"));

	Initialized = true;
}

void Report(const char* format, ...) {
	char buf[8192];
	buf[0] = 0;

	time_t _tm = time(NULL);
	struct tm * curtime = localtime(&_tm);
	char const* t = asctime(curtime);
	strcat(buf, t);
	buf[strlen(buf) - 1] = 0;

	strcat(buf, " | ");

	va_list args;
	va_start(args, format);
	vsprintf(buf + strlen(buf), format, args);
	va_end(args);

	strcat(buf, "\n");

	size_t length = strlen(buf);

	FILE* fp = fopen("R:\\lglog.txt", "a+");
	fwrite(buf, 1, length, fp);
	fclose(fp);
}

CMonitorDDCCISDK::CMonitorDDCCISDK() {
	EnsureInitialized();

	Report("ctor");

	(this->*_ctor)();
}

CMonitorDDCCISDK::~CMonitorDDCCISDK() {
	EnsureInitialized();

	Report("dtor");

	(this->*_dtor)();
}

_T_E_MONITOR_ERROR CMonitorDDCCISDK::InitializeDDCCIStack(int unk) {
	EnsureInitialized();

	Report("InitializeDDCCIStack(%d)", unk);

	return (this->*_init)(unk);
}

_T_E_MONITOR_ERROR CMonitorDDCCISDK::UnInitializeDDCCISDK(int unk) {
	EnsureInitialized();

	Report("UnInitializeDDCCISDK(%d)", unk);

	return (this->*_uninit)(unk);
}

_T_E_MONITOR_ERROR CMonitorDDCCISDK::GetPropertyWithoutOpcodeVerification(enum  T_E_OPCODE opcode, unsigned short& val1, unsigned short& val2) {
	EnsureInitialized();

	auto ret = (this->*_get)(opcode, val1, val2);

	Report("GetPropertyWithoutOpcodeVerification(%d, %d, %d)", opcode, val1, val2);

	return ret;
}

_T_E_MONITOR_ERROR CMonitorDDCCISDK::SetPropertyWithoutOpcodeVerification(enum  T_E_OPCODE opcode, unsigned short val) {
	EnsureInitialized();

	Report("SetPropertyWithoutOpcodeVerification(%d, %d)", opcode, val);

	return (this->*_set)(opcode, val);
}
