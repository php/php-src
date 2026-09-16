--TEST--
CGI/FastCGI: a boundary segment that cannot be fully preallocated fails closed at startup and leaves nothing behind
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip boundary shared memory is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
if (!defined('SIGXFSZ')) die('skip SIGXFSZ not available');
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

function user_cache_cgi_run(string $phpCgi, string $script, string $docRoot, string $lockfilePath, string $query, ?int $fileSizeLimitKiB): array
{
    $command = [
        $phpCgi, '-n', '-q',
        '-d', 'user_cache.enable=1',
        '-d', 'user_cache.shm_size=16M',
        '-d', 'user_cache.lockfile_path=' . $lockfilePath,
        '-d', 'display_errors=0',
        '-d', 'log_errors=1',
        '-d', 'html_errors=0',
    ];
    if ($fileSizeLimitKiB !== null) {
        /* RLIMIT_FSIZE makes every attempt to extend a file past the limit
         * fail with EFBIG, which stands in for a full filesystem. */
        array_unshift($command, 'sh', '-c', 'ulimit -f ' . $fileSizeLimitKiB . ' && exec "$0" "$@"');
    }

    $process = proc_open(
        $command,
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        $docRoot,
        [
            'REDIRECT_STATUS' => '1',
            'REQUEST_METHOD' => 'GET',
            'SCRIPT_FILENAME' => $script,
            'DOCUMENT_ROOT' => $docRoot,
            'SERVER_NAME' => 'preallocate.local',
            'QUERY_STRING' => $query,
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

$root = sys_get_temp_dir() . '/php-user-cache-cgi-preallocate-' . getmypid();
$docRoot = $root . '/doc';
$script = $docRoot . '/index.php';
$lockfilePath = $root . '/lock';

user_cache_cgi_rm_rf($root);
mkdir($docRoot, 0777, true);
mkdir($lockfilePath, 0777);

$privateDir = $lockfilePath . '/.PhpUserCacheBnd.' . fileowner($root);

file_put_contents($script, <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('preallocate');
if (($_GET['action'] ?? 'fetch') === 'seed') {
    var_dump($cache->store('key', 'seeded'));
}
echo UserCache\Cache::getStatus()->getAvailability()->name, ':', $cache->fetch('key', 'MISS'), "\n";
PHP);

/* An ignored disposition survives exec(), so the worker gets EFBIG instead
 * of being killed by SIGXFSZ when preallocation crosses the limit. */
pcntl_signal(SIGXFSZ, SIG_IGN);

try {
    $phpCgi = user_cache_cgi_binary();

    [$stdout, $stderr, $status] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=seed', 1024);
    echo "limited: $stdout\n";
    /* Startup is attempted at request activation and again by the first
     * API call; both attempts fail the same way. */
    echo preg_replace('/^PHP Warning:  .*?(Cache: shared memory initialization failed: [^\n]*?) in .*$/m', '$1', $stderr), "\n";
    var_dump($status);
    var_dump(user_cache_private_dir_entries($privateDir));

    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=seed', null);
    echo "unlimited: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=fetch', null);
    echo "attached: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    var_dump(count(user_cache_private_dir_entries($privateDir)));

    echo "Done\n";
} finally {
    user_cache_cgi_rm_rf($root);
}

?>
--EXPECTF--
limited: UnavailableBySharedMemoryInitializationFailed:MISS
Cache: shared memory initialization failed: preallocate segment: File too large (%d)
UserCache partition startup failed; UserCache will be unavailable
Cache: shared memory initialization failed: preallocate segment: File too large (%d)
int(0)
array(1) {
  [0]=>
  string(4) "salt"
}
unlimited: Available:seeded
attached: Available:seeded
int(3)
Done
