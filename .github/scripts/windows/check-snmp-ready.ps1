param(
    [Parameter(Mandatory = $true)]
    [string]$PhpBuildDir
)

$php = Join-Path $PhpBuildDir 'php.exe'
$snmpExtension = Join-Path $PhpBuildDir 'php_snmp.dll'

if (-not (Test-Path $php)) {
    Write-Error "Could not find $php"
    exit 1
}

if (-not (Test-Path $snmpExtension)) {
    Write-Error "Could not find $snmpExtension"
    exit 1
}

$phpCode = @'
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
$oid = '.1.3.6.1.2.1.1.1.0';
if (@snmpget('127.0.0.1', 'public', $oid, 1000000, 0) === false) {
    exit(1);
}
if (@snmp3_get('127.0.0.1', 'adminMD5AES', 'authPriv', 'MD5', 'test1234', 'AES', 'test1234', $oid, 1000000, 0) === false) {
    exit(1);
}
'@

for ($i = 0; $i -lt 30; $i++) {
    & $php -n "-dextension_dir=$PhpBuildDir" -dextension=php_snmp.dll -r $phpCode *> $null
    if ($LASTEXITCODE -eq 0) {
        exit 0
    }

    if (-not (Get-Process -Name snmpd -ErrorAction SilentlyContinue)) {
        Write-Output 'snmpd exited before the readiness check succeeded'
        exit 1
    }

    Start-Sleep -Seconds 1
}

Write-Output 'snmpd did not become ready within 30 seconds'

exit 1
