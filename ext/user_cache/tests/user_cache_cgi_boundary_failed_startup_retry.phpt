--TEST--
CGI: a request whose boundary partition failed to start at activation stays unavailable, and the next activation starts it with the INI in effect before the [PATH=] section
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
if (PHP_OS_FAMILY === 'Windows') die('skip boundary shared memory is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require_once __DIR__ . '/user_cache_fcgi_tester.inc';
if (FcgiTester::binary() === null) die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

$root = sys_get_temp_dir() . '/php-user-cache-cgi-failed-startup-' . getmypid();
$docRoot = $root . '/doc';
$smallDir = $docRoot . '/small';
$lockfilePath = $root . '/lock';
$ini = $root . '/php.ini';
@mkdir($smallDir, 0777, true);
@mkdir($lockfilePath, 0777, true);

/* The [PATH=] section applies after the startup at activation, so no request may start the segment with it. */
file_put_contents($ini, <<<INI
display_errors=0
user_cache.enable=1
user_cache.shm_size=8M
user_cache.lockfile_path=$lockfilePath
[PATH=$smallDir]
user_cache.shm_size=4M
INI);
$script = <<<'PHP'
<?php
putenv('USER_CACHE_DEBUG_FAIL_BOUNDARY_SEGMENT=0');
$cache = UserCache\Cache::getPool('failed-startup');
echo UserCache\Cache::getStatus()->getSharedMemorySize(), ' ';
if (isset($_GET['store'])) {
    var_dump($cache->store($_GET['store'], 'from-' . basename(__DIR__)));
} else {
    var_dump($cache->fetch($_GET['fetch']));
}
PHP;
file_put_contents($smallDir . '/index.php', $script);
file_put_contents($docRoot . '/index.php', $script);

function run_cgi(string $ini, string $docRoot, string $script, string $query, bool $failStartup): string
{
    $process = proc_open(
        [FcgiTester::binary(), '-q', '-c', $ini],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        dirname($script),
        [
            'REDIRECT_STATUS' => '1',
            'REQUEST_METHOD' => 'GET',
            'QUERY_STRING' => $query,
            'SCRIPT_FILENAME' => $script,
            'DOCUMENT_ROOT' => $docRoot,
            'SERVER_NAME' => 'failed-startup.local',
            'USER_CACHE_DEBUG_FAIL_BOUNDARY_SEGMENT' => $failStartup ? '1' : '0',
        ]
    );
    fclose($pipes[0]);
    $stdout = trim(preg_replace('/\A(?:[A-Za-z-]+: [^\r\n]*\r?\n)+\r?\n/', '', stream_get_contents($pipes[1])));
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($process);

    return $stdout;
}

try {
    echo 'small, failed startup: ', run_cgi($ini, $docRoot, $smallDir . '/index.php', 'store=p', true), "\n";
    echo 'global: ', run_cgi($ini, $docRoot, $docRoot . '/index.php', 'store=q', false), "\n";
    echo 'small: ', run_cgi($ini, $docRoot, $smallDir . '/index.php', 'fetch=q', false), "\n";
    echo 'small, failed startup: ', run_cgi($ini, $docRoot, $smallDir . '/index.php', 'fetch=q', true), "\n";
    echo 'global: ', run_cgi($ini, $docRoot, $docRoot . '/index.php', 'fetch=p', false), "\n";
} finally {
    FcgiTester::removeBoundarySegments($lockfilePath);
    foreach ([$smallDir . '/index.php', $docRoot . '/index.php', $ini] as $file) {
        @unlink($file);
    }
    $private = $lockfilePath . '/.PhpUserCacheBnd.' . fileowner($root);
    array_map('unlink', glob($private . '/{,.}[!.]*', GLOB_BRACE) ?: []);
    @rmdir($private);
    @rmdir($lockfilePath);
    @rmdir($smallDir);
    @rmdir($docRoot);
    @rmdir($root);
}
?>
--EXPECT--
small, failed startup: 0 bool(false)
global: 8388608 bool(true)
small: 8388608 string(8) "from-doc"
small, failed startup: 0 NULL
global: 8388608 NULL
