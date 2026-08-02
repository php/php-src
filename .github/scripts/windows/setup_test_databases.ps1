$ErrorActionPreference = "Stop"

function Invoke-NativeCommand {
    param(
        [string] $Description,
        [string] $FilePath,
        [string[]] $ArgumentList
    )

    & $FilePath @ArgumentList
    if ($LASTEXITCODE -ne 0) {
        throw "$Description exited with code $LASTEXITCODE"
    }
}

$totalStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
$sqlServerStopwatch = [System.Diagnostics.Stopwatch]::StartNew()

Invoke-NativeCommand "SQL Server Express installation" "choco.exe" @(
    "install",
    "sql-server-express",
    "--version=2022.16.0.20260305",
    "-y",
    "--no-progress",
    "--install-arguments=/SECURITYMODE=SQL /SAPWD=Password12!"
)
$sqlServerStopwatch.Stop()
Write-Host "SQL Server Express setup duration: $($sqlServerStopwatch.Elapsed)"

$mysqlStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
Invoke-NativeCommand "MySQL initialization" "mysqld.exe" @("--initialize-insecure")
Invoke-NativeCommand "MySQL service installation" "mysqld.exe" @("--install")
Invoke-NativeCommand "MySQL service startup" "net.exe" @("start", "MySQL")
Invoke-NativeCommand "MySQL root account setup" "mysql.exe" @(
    "--port=3306",
    "--user=root",
    "-e",
    "ALTER USER 'root'@'localhost' IDENTIFIED BY 'Password12!'; FLUSH PRIVILEGES;"
)
$mysqlStopwatch.Stop()
Write-Host "MySQL setup duration: $($mysqlStopwatch.Elapsed)"

$postgresStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
$postgresService = if ($env:PHP_BUILD_CRT -eq "vs18") { "postgresql-x64-17" } else { "postgresql-x64-14" }
Set-Service -Name $postgresService -StartupType Manual -Status Running
$env:PGPASSWORD = "root"
Invoke-NativeCommand "PostgreSQL account setup" "$env:PGBIN\psql.exe" @(
    "-U",
    "postgres",
    "-c",
    "ALTER USER postgres WITH PASSWORD 'Password12!';"
)
$postgresStopwatch.Stop()
Write-Host "PostgreSQL setup duration: $($postgresStopwatch.Elapsed)"

$totalStopwatch.Stop()
Write-Host "Total database setup duration: $($totalStopwatch.Elapsed)"
