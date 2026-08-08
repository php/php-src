$ErrorActionPreference = "Stop"

$databaseSetup = Join-Path $PSScriptRoot "setup_test_databases.ps1"
$databaseJob = Start-Job -FilePath $databaseSetup

try {
    $buildStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    & (Join-Path $PSScriptRoot "build.bat")
    $buildExitCode = $LASTEXITCODE
    $buildStopwatch.Stop()
    Write-Host "PHP build duration: $($buildStopwatch.Elapsed)"

    $databaseWaitStopwatch = [System.Diagnostics.Stopwatch]::StartNew()
    Wait-Job -Job $databaseJob | Out-Null
    $databaseWaitStopwatch.Stop()
    Write-Host "Database wait after PHP build: $($databaseWaitStopwatch.Elapsed)"

    $databaseState = $databaseJob.State
    $databaseError = $databaseJob.ChildJobs[0].JobStateInfo.Reason
    Receive-Job -Job $databaseJob -ErrorAction Continue

    if ($buildExitCode -ne 0) {
        throw "PHP build exited with code $buildExitCode"
    }
    if ($databaseState -ne "Completed") {
        throw "Test database setup failed: $databaseError"
    }
} finally {
    if ($databaseJob.State -eq "Running") {
        Stop-Job -Job $databaseJob
    }
    Remove-Job -Job $databaseJob
}
