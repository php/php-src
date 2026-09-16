--TEST--
CGI/FastCGI: a boundary segment larger than its filesystem fails closed at startup (ENOSPC) instead of raising SIGBUS later
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip boundary shared memory is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require __DIR__ . '/small_filesystem.inc';
if (user_cache_small_filesystem() === null) {
    die('skip no small filesystem: set TEST_PHP_USER_CACHE_SMALL_FS to a writable directory on one, or run with a small /dev/shm');
}
$root = dirname(__DIR__, 3);
$candidates = [
    getenv('TEST_PHP_CGI_EXECUTABLE') ?: null,
    $root . '/sapi/cgi/php-cgi',
];
foreach ($candidates as $candidate) {
    if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
        return;
    }
}
die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require __DIR__ . '/small_filesystem.inc';

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

function user_cache_cgi_run(string $phpCgi, string $script, string $docRoot, string $lockfilePath, string $shmSize): array
{
    $process = proc_open(
        [
            $phpCgi, '-n', '-q',
            '-d', 'user_cache.enable=1',
            '-d', 'user_cache.shm_size=' . $shmSize,
            '-d', 'user_cache.lockfile_path=' . $lockfilePath,
            '-d', 'display_errors=0',
            '-d', 'log_errors=1',
            '-d', 'html_errors=0',
        ],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        $docRoot,
        [
            'REDIRECT_STATUS' => '1',
            'REQUEST_METHOD' => 'GET',
            'SCRIPT_FILENAME' => $script,
            'DOCUMENT_ROOT' => $docRoot,
            'SERVER_NAME' => 'nospace.local',
        ]
    );
    if (!is_resource($process)) {
        throw new RuntimeException('Unable to start php-cgi');
    }

    fclose($pipes[0]);
    $stdout = stream_get_contents($pipes[1]);
    $stderr = stream_get_contents($pipes[2]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    $status = proc_close($process);

    $lines = preg_split('/\r?\n/', trim($stdout));

    return [end($lines), trim($stderr), $status];
}

function user_cache_cgi_rm_rf(string $path): void
{
    if (!file_exists($path) && !is_link($path)) {
        return;
    }

    if (!is_dir($path) || is_link($path)) {
        unlink($path);
        return;
    }

    foreach (scandir($path) as $entry) {
        if ($entry === '.' || $entry === '..') {
            continue;
        }
        user_cache_cgi_rm_rf($path . DIRECTORY_SEPARATOR . $entry);
    }

    rmdir($path);
}

function user_cache_private_dir_entries(string $dir): array
{
    $entries = scandir($dir);

    return $entries === false ? [] : array_values(array_diff($entries, ['.', '..']));
}

[$smallFs, $shmSize] = user_cache_small_filesystem();

$root = sys_get_temp_dir() . '/php-user-cache-cgi-nospace-' . getmypid();
$docRoot = $root . '/doc';
$script = $docRoot . '/index.php';
$lockfilePath = $smallFs . '/php-user-cache-cgi-nospace-' . getmypid();

user_cache_cgi_rm_rf($root);
user_cache_cgi_rm_rf($lockfilePath);
mkdir($docRoot, 0777, true);
mkdir($lockfilePath, 0777);

$privateDir = $lockfilePath . '/.PhpUserCacheBnd.' . fileowner($root);

file_put_contents($script, <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('nospace');
/* 1 MiB values: on a sparse segment one of these stores would hit the
 * unbacked page and die with SIGBUS. A fitting 2M segment only holds one
 * value at a time, so LRU eviction leaves just the most recent key behind:
 * probe that one rather than key0. */
$last = null;
for ($i = 0; $i < 64; $i++) {
    if (!$cache->store("key$i", str_repeat('x', 1024 * 1024))) {
        break;
    }
    $last = "key$i";
}
echo UserCache\Cache::getStatus()->getAvailability()->name, ':', $last !== null && is_string($cache->fetch($last)) ? 'HIT' : 'MISS', "\n";
PHP);

try {
    $phpCgi = user_cache_cgi_binary();

    [$stdout, $stderr, $status] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, $shmSize);
    echo "oversized: $stdout\n";
    /* Startup is attempted at request activation and again by the first
     * API call; both attempts fail the same way. */
    echo preg_replace('/^PHP Warning:  .*?(Cache: shared memory initialization failed: [^\n]*?) in .*$/m', '$1', $stderr), "\n";
    var_dump($status);
    var_dump(user_cache_private_dir_entries($privateDir));

    [$stdout, $stderr, $status] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, '2M');
    echo "fitting: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    var_dump($status);

    echo "Done\n";
} finally {
    user_cache_cgi_rm_rf($root);
    user_cache_cgi_rm_rf($lockfilePath);
}

?>
--EXPECTF--
oversized: UnavailableBySharedMemoryInitializationFailed:MISS
Cache: shared memory initialization failed: preallocate segment: No space left on device (%d)
UserCache partition startup failed; UserCache will be unavailable
Cache: shared memory initialization failed: preallocate segment: No space left on device (%d)
int(0)
array(1) {
  [0]=>
  string(4) "salt"
}
fitting: Available:HIT
int(0)
Done
