#pragma comment(lib, "advapi32")

#if defined(IDA_DB_IS_32_BIT)
#pragma comment(lib, "idasdk/lib/x64_win_vc_32/ida.lib")
#elif defined(IDA_DB_IS_64_BIT)
#define __EA64__
#pragma comment(lib, "idasdk/lib/x64_win_vc_64/ida.lib")
#else
#error Is IDA database 32 or 64 bit?
#endif

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>

#pragma warning(disable: 4244 4267)

#include <ida.hpp>
#include <idp.hpp>
#include <dbg.hpp>
#include <loader.hpp>
#include <kernwin.hpp>

const auto MSG_PREFIX = "[ScyllaHide for IDA 7] ";

constexpr auto PREFIX_X86 = L"x86";
constexpr auto PREFIX_X64 = L"x64";

static HMODULE thisInstance;
static WCHAR   thisInstanceDir[MAX_PATH];

/// <summary>
/// Get directory of current plugin, ending with "\".
/// <summary>
static WCHAR* GetThisInstanceDir()
{
	int len = GetModuleFileNameW(thisInstance, thisInstanceDir, MAX_PATH - 2);
	while (len)
	{
		if (thisInstanceDir[len - 1] == '\\')
		{
			thisInstanceDir[len] = 0;
			break;
		}

		len--;
	}
	return thisInstanceDir;
}

static bool SetDebugPrivileges()
{
	TOKEN_PRIVILEGES Debug_Privileges;
	bool retVal = false;

	if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Debug_Privileges.Privileges[0].Luid))
	{
		HANDLE hToken = 0;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		{
			Debug_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			Debug_Privileges.PrivilegeCount = 1;

			retVal = AdjustTokenPrivileges(hToken, FALSE, &Debug_Privileges, 0, NULL, NULL) != FALSE;

			CloseHandle(hToken);
		}
	}

	return retVal;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		thisInstance = hModule;
		GetThisInstanceDir();

		SetDebugPrivileges();
		break;
	}
	return TRUE;
}

static WCHAR* GetInjectorCliPath(bool is32)
{
	static WCHAR InjectorCliPath[MAX_PATH];
	auto prefix = is32 ? PREFIX_X86 : PREFIX_X64;
	swprintf_s(InjectorCliPath, L"InjectorCLI%s.exe", prefix);
	return InjectorCliPath;
}

static WCHAR* GetHookLibraryPath(bool is32)
{
	static WCHAR HookLibraryPath[MAX_PATH];
	auto prefix = is32 ? PREFIX_X86 : PREFIX_X64;
	swprintf_s(HookLibraryPath, L"HookLibrary%s.dll", prefix);
	return HookLibraryPath;
}

static void InjectHiderIntoDebuggee(pid_t pid, bool isDebuggee32bit)
{
	WCHAR cmd[MAX_PATH * 2] = { 0 };
	swprintf_s(
		cmd,
		L"\"%s%s\" pid:%d \"%s%s\"",
		thisInstanceDir, GetInjectorCliPath(isDebuggee32bit),
		pid,
		thisInstanceDir, GetHookLibraryPath(isDebuggee32bit)
	);

	STARTUPINFO si = {};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW; // which is SW_HIDE

	PROCESS_INFORMATION pi = {};

	if (CreateProcessW(NULL, cmd, NULL, NULL, false, 0, NULL, NULL, &si, &pi))
	{
		msg(MSG_PREFIX);
		msg("Injector started...\n");

		WaitForSingleObject(pi.hProcess, 1000);

		// Now CLI Injector wait for Enter.
		// Just kill it (need proper fix later).
		TerminateProcess(pi.hProcess, 0);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		/*
		DWORD exitCode = 0;
		auto gotExitCode = GetExitCodeProcess(pi.hProcess, &exitCode);

		msg(MSG_PREFIX);
		msg("Exit code: %d\n", exitCode);
		*/
	}
	else
	{
		msg(MSG_PREFIX);
		msg("Injector start failed\n");
	}
}

static bool IsDebuggee32bit()
{
	return inf.is_32bit() && !inf.is_64bit();
}

static ssize_t idaapi DebugCallback(void *user_data, int notif_code, va_list va)
{
	switch (notif_code)
	{
	case dbg_process_start:
	case dbg_process_attach:
		const debug_event_t* dbgEvent = va_arg(va, const debug_event_t*);
		InjectHiderIntoDebuggee(dbgEvent->pid, IsDebuggee32bit());
		break;
	}

	return 0;
}

static int idaapi IDAP_init()
{
	//if (inf.filetype != f_PE)
	//	return PLUGIN_SKIP;

	if (!hook_to_notification_point(HT_DBG, DebugCallback))
		return PLUGIN_SKIP;

	return PLUGIN_KEEP;
}

void idaapi IDAP_term()
{
	unhook_from_notification_point(HT_DBG, DebugCallback);
}

bool idaapi IDAP_run(size_t arg)
{
	return true;
}

idaman ida_module_data plugin_t PLUGIN =
{
	IDP_INTERFACE_VERSION,
	PLUGIN_FIX,
	IDAP_init,
	IDAP_term,
	IDAP_run,
	"ScyllaHide Comment",
	"ScyllaHide Help",
	"ScyllaHide For IDA 7",
	"Alt-X"
};