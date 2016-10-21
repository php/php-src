<#
.SYNOPSIS
	Compile PHP.
.DESCRIPTION
	This Powershell script compiles (or recompiles) PHP for Windows, in 32 or 64 bits.
.PARAMETER Bits
	32 to built for 32-bit PCs, 64 to build for 64-bit PCs.
	By default, if an environment variable named "Platform" exists and evaluates to "x86" or "x64", we'll use it.
	Otherwise this value will be 32 if the current PC is 32-bit or 64 if the current PC is 64-bit.
	Subsequent calls to this script will re-use the previously number of bits (if not specified in the Bits parameter).
.NOTES
	Author: Michele Locati <mlocati@gmail.com>
#>

# Set-ExecutionPolicy -ExecutionPolicy Unrestricted -Scope Process -Force

param([Byte]$bits=0)

# Setup preferences variables

$ErrorActionPreference = "Stop"
$WarningPreference = "Stop"
$ConfirmPreference = "None"
$ProgressPreference = "SilentlyContinue"

# Define some functions

function GetArchive([string]$remote, [string]$local) {
	if (!(Test-Path -Path $local)) {
		Write-Host -NoNewline "Downloading: $remote... "
		Invoke-WebRequest -Uri $remote -Method "Get" -OutFile $local
		Write-Host "done."
	}
}
function Unzip([string]$zipFile, [string]$destinationDirectory) {
	if (!(Test-Path -Path $destinationDirectory)) {
		Write-Host -NoNewline "Extracting: $zipFile... "
		Add-Type -AssemblyName "System.IO.Compression.FileSystem"
		[System.IO.Compression.ZipFile]::ExtractToDirectory($zipFile, $destinationDirectory)
		Write-Host "done."
	}
}

function Un7z([string]$7zipFile, [string]$destinationDirectory) {
	Write-Host -NoNewline "Extracting: $7zipFile... "
	& $7zipExe x -bd -y -o"$destinationDirectory" "$7zipFile" | Out-Null
	if (!($?)) {
		Write-Host "failed!"
		exit 1
	}
	Write-Host "done."
}

# Setup variables
$scriptDirectory = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent
$sourceDirectory = Split-Path -Path $scriptDirectory -Parent
$tempDirectory = Join-Path -Path $scriptDirectory -ChildPath "temp"
$lastBuiltBitsFile = Join-Path -Path $tempDirectory -ChildPath "last.bits"
$previousBits = 0
if (Test-Path -Path $lastBuiltBitsFile) {
	switch (Get-Content -LiteralPath $lastBuiltBitsFile -ReadCount 1) { 
		"32" {
			$previousBits = 32
		}
		"64" {
			$previousBits = 64
		}
	}
}
if ($bits -eq 0) {
	if ($previousBits -eq 0) {
		if ($env:Platform -eq "x64") {
			$bits = 64
		} elseif ($env:Platform -eq "x86") {
			$bits = 32
		} elseif ($env:PROCESSOR_ARCHITECTURE -eq "AMD64") {
			$bits = 64
		} else {
			$bits = 32
		}
	} else {
		$bits = $previousBits
	}
}
switch ($bits) { 
	32 {
		$architectureName = "x86"
		$architectureName2 = "x86"
	}
	64 {
		$architectureName = "x64"
		$architectureName2 = "amd64"
	}
	default {
		Write-Host "Invalid number of bits: $bits"
		exit 1
	}
}
$archivesDirectory = Join-Path -Path $tempDirectory -ChildPath "archives"
$7zipArchive = Join-Path -Path $archivesDirectory -ChildPath "7zip.zip"
$7zipFolder = Join-Path -Path $archivesDirectory -ChildPath "7zip"
$7zipExe = Join-Path -Path $7zipFolder -ChildPath "7za.exe"
$toolsArchive = Join-Path -Path $archivesDirectory -ChildPath "php-sdk-binary-tools.zip"
$toolsDirectory = Join-Path -Path $tempDirectory -ChildPath "tools"
$dependenciesArchive = Join-Path -Path $archivesDirectory -ChildPath "deps-$architectureName.7z"
$dependenciesDirectory = Join-Path -Path $tempDirectory -ChildPath "deps-$architectureName"
$objOutDirectory = Join-Path -Path $tempDirectory -ChildPath "obj-$architectureName"
$visualStudioRoot=$env:VS140COMNTOOLS
$visualStudioRoot = Split-Path -Path $visualStudioRoot -Parent
$visualStudioRoot = Split-Path -Path $visualStudioRoot -Parent
$vcvarsall = Join-Path -Path $visualStudioRoot -ChildPath "VC"
$vcvarsall = Join-Path -Path $vcvarsall -ChildPath "vcvarsall.bat"
$outputDirectory = Join-Path -Path $scriptDirectory -ChildPath "out-$architectureName"
$scriptFile = Join-Path -Path $tempDirectory -ChildPath "compile.cmd"
$configure = $true
if (Test-Path $outputDirectory) {
	if ($bits -eq $previousBits) {
		$configure = $false
	} else {
		Remove-Item -Recurse -Force -LiteralPath $outputDirectory
	}
}
if ($configure) {
	if (Test-Path -Path $objOutDirectory) {
		Remove-Item -Recurse -LiteralPath $objOutDirectory
	}
	if (Test-Path -Path $lastBuiltBitsFile) {
		Remove-Item -LiteralPath $lastBuiltBitsFile
	}
}
if (!(Test-Path -Path $objOutDirectory)) {
	New-Item -Path $objOutDirectory -ItemType "directory" | Out-Null
}

# Prepare the required tools and dependencies
if (!(Test-Path -Path $tempDirectory)) {
	New-Item -Path $tempDirectory -ItemType "directory" | Out-Null
}
if (!(Test-Path -Path $archivesDirectory)) {
	New-Item -Path $archivesDirectory -ItemType "directory" | Out-Null
}
GetArchive "http://www.7-zip.org/a/7za920.zip" $7zipArchive
GetArchive "http://windows.php.net/downloads/php-sdk/php-sdk-binary-tools-20110915.zip" $toolsArchive
GetArchive "http://windows.php.net/downloads/php-sdk/deps-master-vc14-$architectureName.7z" $dependenciesArchive

# Prepare 7-zip
Unzip $7zipArchive $7zipFolder

# Decompress the PHP SDK Binary tools
Unzip $toolsArchive $toolsDirectory

# Decompress dependencies archive
if (!(Test-Path -Path $dependenciesDirectory)) {
	Un7z $dependenciesArchive $archivesDirectory
	$un7z = Join-Path -Path $archivesDirectory -ChildPath "deps"
	Move-Item -Path $un7z -Destination $dependenciesDirectory
}

# Build the build script
$batch = @"
@echo off
setlocal

set PATH=%SystemRoot%\System32;%SystemRoot%

if "%APPVEYOR%" equ "True" rmdir /s /q C:\cygwin >NUL 2>NUL

call `"$vcvarsall`" $architectureName2
if errorlevel 1 exit /b 1
call `"$toolsDirectory\bin\phpsdk_setvars.bat`"
if errorlevel 1 exit /b 1
cd /D `"$sourceDirectory`"
if errorlevel 1 exit /b 1
"@
if ($configure) {
	$batch = @"
$batch
nmake clean >NUL 2>NUL
call buildconf.bat --force
if errorlevel 1 exit /b 1
mkdir "$outputDirectory"
if errorlevel 1 exit /b 1
call configure.bat ^
	--enable-snapshot-build ^
	--enable-debug-pack ^
	--enable-com-dotnet=shared ^
	--with-mcrypt=static ^
	--without-analyzer ^
	`"--enable-object-out-dir=$objOutDirectory`" ^
	`"--with-prefix=$outputDirectory`" ^
	`"--with-php-build=$dependenciesDirectory`" ^
	--with-mp=auto
if errorlevel 1 exit /b 1
echo $bits>`"$lastBuiltBitsFile`"

"@
}
$batch = @"
$batch

nmake /NOLOGO /S
if errorlevel 1 exit /b 1

nmake /NOLOGO /S install
if errorlevel 1 exit /b 1

exit /b 0
"@

if ($configure) {
	Write-Host "Configuring and compiling for $bits bits"
} else {
	Write-Host "Compiling for $bits bits"
}
$batch | Out-File -FilePath $scriptFile -Encoding "ascii"

$ErrorActionPreference = "SilentlyContinue"
& $scriptFile
$scriptExitCode = $LastExitCode
$ErrorActionPreference = "Stop"
Remove-Item -LiteralPath $scriptFile
if ($scriptExitCode -ne 0) {
	Write-Host "Build script failed!"
	exit 1
}
