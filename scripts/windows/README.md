# Windows build scripts

Originally used only for GitHub actions, these scripts can help windows users to be able to build locally.
While still very new (and open to feedback and [pull requests](https://github.com/php/php-src/fork))

## Pre-requisites

- Windows 2019 or newer [Microsoft](https://www.microsoft.com/en-us/evalcenter/download-windows-server-2019)
- Administrative access
- Chocolatey installed to use as a package manager [Install instructions](https://chocolatey.org/install)
- MySQL installed [see our CI setup](.github/actions/setup-windows/action.yml)
- SQL Server Installed [see our CI setup](.github/actions/setup-windows/action.yml)
- Postgres Installed [see our CI setup](.github/actions/setup-windows/action.yml)
- Git installed
- Unzip utility installed
- 7zip utility installed
- Mirosoft Visual Studio 2019 Community or newer [Microsoft](https://visualstudio.microsoft.com/vs/older-downloads/)

## Intended order of operations

1. Setup pre-requisites
2. Setup environment [env.bat](./scripts/windows/env.bat)
3. Build PHP [build.bat](./scripts/windows/build.bat)
4. Run PHP Tests [test.bat](./scripts/windows/test.bat)
5. Cleanup [cleanup.bat](./scripts/windows/cleanup.bat)

## env.bat

The purpose of this script is simply to setup environment. It is intended to be run via command-line `cmd` under windows as an administrator.
You can adjust values in this script to:

- Get more workers (theoretically runs tests faster)
- Adjust temporary and working diretories (although It is untested)
- Change between platform target (32-bit vs 64-bit)
- Change Branch
- Change PHP build SDK
- Masquerades as being inside a CI environment so the scripts do not exit early

In the future, it would be nice if some configuration for removing extension enabling and disabling, and selecting components, such as `mysql`, `mssql`, `psql`, `firebird`, `snmp` became optional via this script.

## build.bat

The purpose of this script is to compile PHP, after setting up environment. It is intended to be run via command-line `cmd` under windows as an administrator.

It coordinates running `build_task.bat`. The reason for that separation is unclear at this time, but build does pre-flight checks and downloads things according to the env. `build_task.bat` then performs the build.

## test.bat

The purpose of this script is to test PHP, after setting up environment, and compiling. It is intended to be run via command-line `cmd` under windows as an administrator.

It coordinates running `test_task.bat`. The reason for that separation is unclear at this time. 

## test_task.bat

The purpose of this script is to test PHP, after setting up environment, and compiling. It is intended to be run via test.bat and requires administrative privileges to operate successfully.

This file has a lot of logic. In addition to building the tests, it performs setup of PHP ini, modifications to configuration files for tests, and even fetching dependencies.

## cleanup.bat

The purpose of this script is to shutdown less common services which tests instantiate. Test instance of Firebird and SNMPTrap, as well as clearing temporary files.

The script is a work in progress based on one engineer machine. Please contribute to improve this.
