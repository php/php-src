# https://learn.microsoft.com/en-us/troubleshoot/sql/azure-sql/sql-installation-fails-sector-size-error-azure-vm
$ErrorActionPreference = 'Stop'

$compatibleDrives = foreach ($driveLetter in 'C', 'D') {
    if (!(Test-Path "${driveLetter}:\")) {
        continue
    }
    try {
        $disk = Get-Partition -DriveLetter $driveLetter | Get-Disk
    } catch {
        Write-Warning "Cannot determine the sector size of ${driveLetter}: $_"
        continue
    }
    if ($disk.PhysicalSectorSize -in @(512, 4096)) {
        $driveLetter
    }
}
$driveLetter = $compatibleDrives | Select-Object -First 1
if (!$driveLetter) {
    throw 'Neither C: nor D: has a SQL Server-compatible physical sector size (512 or 4096 bytes).'
}
$installDir = "${driveLetter}:\SQLServer"
Write-Host "Installing SQL Server in $installDir"

$delay = 3
for ($attempt = 1; $attempt -le 3; $attempt++) {
    choco install sql-server-express -y --no-progress --install-arguments="/SECURITYMODE=SQL /SAPWD=Password12! /INSTANCEDIR=$installDir /INSTALLSQLDATADIR=$installDir"
    $exitCode = $LASTEXITCODE
    if ($exitCode -eq 0) {
        exit 0
    }
    if ($exitCode -notin @(-1, 1, 404) -or (Test-Path $installDir) -or $attempt -eq 3) {
        exit $exitCode
    }
    Write-Host "Retrying SQL Server setup in $delay seconds..."
    Start-Sleep -Seconds $delay
    $delay *= 2
}
