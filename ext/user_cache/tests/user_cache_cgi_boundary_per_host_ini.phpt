--TEST--
CGI: a per-host INI section that changes user_cache.entries_hint does not break writes of the process that created the boundary segment
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip boundary shared memory is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
$root = dirname(__DIR__, 3);
foreach ([getenv('TEST_PHP_CGI_EXECUTABLE') ?: null, $root . '/sapi/cgi/php-cgi'] as $candidate) {
    if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
        return;
    }
}
die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

function user_cache_cgi_binary(): string
{
    $root = dirname(__DIR__, 3);
    foreach ([getenv('TEST_PHP_CGI_EXECUTABLE') ?: null, $root . '/sapi/cgi/php-cgi'] as $candidate) {
        if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
            return $candidate;
        }
    }

    throw new RuntimeException('CGI SAPI binary not available');
}

$root = sys_get_temp_dir() . '/php-user-cache-cgi-per-host-ini-' . getmypid();
$docRoot = $root . '/doc';
$lockfilePath = $root . '/lock';
$script = $docRoot . '/index.php';
$ini = $root . '/php.ini';
@mkdir($docRoot, 0777, true);
@mkdir($lockfilePath, 0777, true);

/* The boundary segment is named during startup, before the [HOST=] section applies. */
file_put_contents($ini, <<<INI
user_cache.enable=1
user_cache.shm_size=8M
user_cache.lockfile_path=$lockfilePath
[HOST=per-host-ini.local]
user_cache.entries_hint=5000
INI);
file_put_contents($script, <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('per-host-ini');
echo ini_get('user_cache.entries_hint'), ' ', var_export($cache->store('pid', getmypid()), true), ' ', var_export($cache->fetch('pid') === getmypid(), true), "\n";
PHP);

try {
    for ($i = 1; $i <= 2; $i++) {
        $process = proc_open(
            [user_cache_cgi_binary(), '-q', '-c', $ini],
            [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
            $pipes,
            $docRoot,
            [
                'REDIRECT_STATUS' => '1',
                'REQUEST_METHOD' => 'GET',
                'SCRIPT_FILENAME' => $script,
                'DOCUMENT_ROOT' => $docRoot,
                'SERVER_NAME' => 'per-host-ini.local',
            ]
        );
        fclose($pipes[0]);
        $lines = preg_split('/\r?\n/', trim(stream_get_contents($pipes[1])));
        $stderr = trim(stream_get_contents($pipes[2]));
        fclose($pipes[1]);
        fclose($pipes[2]);
        proc_close($process);
        echo "run $i: ", end($lines), $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    }
} finally {
    FcgiTester::removeBoundarySegments($root);
    foreach ([$script, $ini] as $file) {
        @unlink($file);
    }
    $private = $lockfilePath . '/.PhpUserCacheBnd.' . fileowner($root);
    array_map('unlink', glob($private . '/{,.}[!.]*', GLOB_BRACE) ?: []);
    @rmdir($private);
    @rmdir($lockfilePath);
    @rmdir($docRoot);
    @rmdir($root);
}
?>
--EXPECT--
run 1: 5000 true true
run 2: 5000 true true
