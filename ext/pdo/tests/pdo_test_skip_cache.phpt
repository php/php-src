--TEST--
PDO test helper caches connection failures for one test run
--EXTENSIONS--
pdo
--FILE--
<?php
function run_pdo_skip_check(string $code, array $environment): string
{
    $command = getenv('TEST_PHP_EXECUTABLE_ESCAPED')
        . ' '
        . getenv('TEST_PHP_EXTRA_ARGS')
        . ' -d opcache.enable_cli=0 -d opcache.jit_buffer_size=0 -r '
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

$dsn = 'missing_' . getmypid() . ':';
$cacheDirectory = __DIR__ . '/pdo_test_skip_cache_' . getmypid();
mkdir($cacheDirectory);

$environment = getenv();
$environment['PDOTEST_DSN'] = $dsn;
$environment['PDOTEST_USER'] = 'test';
$environment['PDOTEST_PASS'] = 'test';
$environment['TEST_PHP_SHARED_CACHE_DIR'] = $cacheDirectory;
unset($environment['PDOTEST_ATTR']);

$helperDirectory = getenv('REDIR_TEST_DIR') ?: __DIR__;
$helper = var_export($helperDirectory . '/pdo_test.inc', true);
$code = "require $helper; PDOTest::skip();";
try {
    $first = run_pdo_skip_check($code, $environment);

    $cacheFiles = glob($cacheDirectory . '/pdo-*');
    if (count($cacheFiles) !== 1) {
        throw new Exception('Expected exactly one cache file');
    }
    $cacheFile = $cacheFiles[0];
    $cachedReason = file_get_contents($cacheFile);
    file_put_contents($cacheFile, 'cached connection failure');

    $second = run_pdo_skip_check($code, $environment);
    echo "$first\n$cachedReason\n$second\n";
} finally {
    foreach (glob($cacheDirectory . '/*') as $file) {
        unlink($file);
    }
    rmdir($cacheDirectory);
}
?>
--EXPECT--
skip could not find driver
could not find driver
skip cached connection failure
