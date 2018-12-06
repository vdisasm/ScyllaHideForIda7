# ScyllaHide for IDA 7
Experimental IDA7 support

# Setup

- Clone repository
- Copy include and libs from IDA SDK
  - idasdk
    - include
    - lib
      - x64_win_vc_32
      - x64_win_vc_64
- Open solution ``ScyllaHideForIda7Plugin\ScyllaHideForIda7Plugin.sln``
- Choose Release configuration (either **IDA64-32** or **IDA64-64**)
  - It means IDA 64 bit (since IDA 7.0) for 32-bit or 64-bit size databases.
    > ida.exe for .idb and ida64.exe for .i64 <br/>
    > Note that ida64.exe can load both 32 and 64 bit executables
   - Plugin naming changed since IDA 7
     - name.dll (for 32-bit db)
     - name64.dll (for 64-bit db)
   - win32_user debugger plugin has bugs for 32-bit version,
     so consider using 64-bit debugger if you can or upgrade IDA.
     I've tried to workaround it (see PatchLocalWin32DebuggerPlugin function),
     but not sure if it's proper fix. So use it on your own risk.
- Build it
- There is Post Build Event for Debug configurations to copy plugin to %IDA%\plugins folder

```
    copy "$(TargetDir)$(TargetFileName)" "D:\Apps\IDA 7.0\plugins\"
```

  Modify path as needed or remove this event

# Deployment

You need to deploy following files:

- HookLibraryx64.dll
- HookLibraryx86.dll
- InjectorCLIx64.exe
- InjectorCLIx86.exe
- NtApiCollection.ini
- scylla_hide.ini
- ScyllaHideForIda7Plugin.dll
- ScyllaHideForIda7Plugin64.dll

# How it works

It uses InjectorCLI to inject HookLibrary when debugger starts or attaches.

No remote debugging is supported and no GUI options.
For now change parameters manually in scylla_hide.ini
