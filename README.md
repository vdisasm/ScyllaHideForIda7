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
- For now there is only Debug IDA64-64 configuration
  - It means IDA 64 bit (since IDA 7.0) for 64-bit size databases (i64, use ida64.exe)
  - Note that ida64.exe can load both 32 and 64 bit executables
- Build it
- There is Post Build Event to copy plugin to %IDA%\plugins folder

```
    copy "$(TargetDir)$(TargetFileName)" "D:\Apps\IDA 7.0\plugins\"
```

  Modify path as needed or remove this event

# Misc

IDA 7 plugin naming scheme changed. Now it is:

```
name.dll
name64.dll
```

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
