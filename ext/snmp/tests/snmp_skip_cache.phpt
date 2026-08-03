--TEST--
SNMP test helper caches agent availability for one test run
--EXTENSIONS--
snmp
--FILE--
<?php
function run_snmp_skip_check(string $code, array $environment): string
{
    $command = getenv('TEST_PHP_EXECUTABLE_ESCAPED')
        . ' '
        . getenv('TEST_PHP_EXTRA_ARGS')
        . ' -r '
        . escapeshellarg($code);
    $process = proc_open(
        $command,
        [
            1 => ['pipe', 'w'],
            2 => ['redirect', 1],
        ],
        $pipes,
        null,
        $environment,
        ['bypass_shell' => true],
    );
    $output = stream_get_contents($pipes[1]);
    fclose($pipes[1]);

    if (0 !== $exitCode = proc_close($process)) {
        throw new Exception("PHP subprocess exited with code $exitCode: $output");
    }

    return $output;
}

$cacheDirectory = __DIR__ . '/snmp_skip_cache_' . getmypid();
mkdir($cacheDirectory);

$environment = getenv();
$environment['SNMP_HOSTNAME'] = '127.0.0.1';
$environment['SNMP_COMMUNITY'] = 'php_test_cache';
$environment['SNMP_TIMEOUT'] = '100000';
$environment['SNMP_RETRIES'] = '0';
$environment['TEST_PHP_SHARED_CACHE_DIR'] = $cacheDirectory;

$helper = var_export(__DIR__ . '/skipif.inc', true);
$code = "require $helper; echo \"available\\n\";";
try {
    $first = run_snmp_skip_check($code, $environment);

    $cacheFiles = glob($cacheDirectory . '/snmp-*');
    if (count($cacheFiles) !== 1) {
        throw new Exception('Expected exactly one cache file');
    }
    $cacheFile = $cacheFiles[0];
    $cachedReason = file_get_contents($cacheFile);
    file_put_contents($cacheFile, 'cached agent failure');

    $second = run_snmp_skip_check($code, $environment);
    echo "$first\n";
    echo "$cachedReason\n";
    echo $second;
} finally {
    foreach (glob($cacheDirectory . '/*') as $file) {
        unlink($file);
    }
    rmdir($cacheDirectory);
}
?>
--EXPECT--
skip NO SNMPD on this host or community invalid
NO SNMPD on this host or community invalid
skip cached agent failure
