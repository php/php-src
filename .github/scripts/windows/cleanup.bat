@echo off
echo "Stopping Firebird Server..."
sc query FirebirdServerTestInstance >nul 2>&1
if errorlevel 1 (
    echo "FirebirdServerTestInstance Service does not exist. Nothing to do."
) else (
	sc stop FirebirdServerTestInstance
	sc delete FirebirdServerTestInstance
	echo "Stopped FirebirdServerTestInstance"
)

echo "Stopping SNMPTrap..."
sc query SNMPTrap >nul 2>&1
if errorlevel 1 (
    echo "SNMPTrap Service does not exist. Nothing to do."
) else (
	sc stop SNMPTrap
	sc delete SNMPTrap
	echo "Stopped SNMPTrap"
)

if EXIST C:\tests_tmp rmdir /s /q C:\tests_tmp
if EXIST %PHP_BUILD_DIR%\test_file_cache rmdir /s /q %PHP_BUILD_DIR%\test_file_cache

