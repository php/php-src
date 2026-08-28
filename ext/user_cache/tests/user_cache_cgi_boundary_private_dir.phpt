--TEST--
CGI/FastCGI: boundary rendezvous objects live in a per-uid private directory and fail closed when it is squatted
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip boundary shared memory is not supported on Windows');
if (!function_exists('proc_open')) die('skip proc_open() not available');
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

function user_cache_cgi_run(string $phpCgi, string $script, string $docRoot, string $lockfilePath, string $query): array
{
    $process = proc_open(
        [
            $phpCgi, '-n', '-q',
            '-d', 'user_cache.enable=1',
            '-d', 'user_cache.shm_size=16M',
            '-d', 'user_cache.lockfile_path=' . $lockfilePath,
            '-d', 'display_errors=0',
            '-d', 'log_errors=0',
        ],
        [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
        $pipes,
        $docRoot,
        [
            'REDIRECT_STATUS' => '1',
            'REQUEST_METHOD' => 'GET',
            'SCRIPT_FILENAME' => $script,
            'DOCUMENT_ROOT' => $docRoot,
            'SERVER_NAME' => 'private-dir.local',
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
    proc_close($process);

    /* A warning raised during request startup flushes the CGI headers
     * before -q applies; the script body is the last line either way. */
    $lines = preg_split('/\r?\n/', trim($stdout));

    return [end($lines), trim($stderr)];
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

$root = sys_get_temp_dir() . '/php-user-cache-cgi-private-dir-' . getmypid();
$docRoot = $root . '/doc';
$script = $docRoot . '/index.php';

user_cache_cgi_rm_rf($root);
mkdir($docRoot, 0777, true);

/* A directory created by this process is owned by the effective uid. */
$uid = fileowner($root);
$privateName = '.ZendUserCacheBnd.' . $uid;

file_put_contents($script, <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('private-dir');
if (($_GET['action'] ?? 'fetch') === 'seed') {
    $cache->store('key', 'seeded');
}
echo UserCache\Cache::getStatus()->getAvailability()->name, ':', $cache->fetch('key', 'MISS'), "\n";
PHP);

try {
    $phpCgi = user_cache_cgi_binary();

    /* Healthy startup: two independently started processes rendezvous
     * through the private directory. */
    $lockfilePath = $root . '/lock';
    mkdir($lockfilePath, 0777);

    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=seed');
    echo "first: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";
    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=fetch');
    echo "second: $stdout", $stderr !== '' ? " [stderr: $stderr]" : '', "\n";

    $privateDir = $lockfilePath . '/' . $privateName;
    var_dump(is_dir($privateDir) && !is_link($privateDir));
    var_dump(fileowner($privateDir) === $uid);
    var_dump(sprintf('%04o', fileperms($privateDir) & 0777));

    $entries = user_cache_private_dir_entries($privateDir);
    sort($entries);
    var_dump(count($entries));
    var_dump(filesize($privateDir . '/salt'));
    var_dump(sprintf('%04o', fileperms($privateDir . '/salt') & 0777));
    var_dump(preg_match('/^[0-9a-f]{24}\.lock$/', $entries[0]) === 1);
    var_dump(sprintf('%04o', fileperms($privateDir . '/' . $entries[0]) & 0777));
    /* Nothing but the private directory lands in user_cache.lockfile_path. */
    var_dump(user_cache_private_dir_entries($lockfilePath) === [$privateName]);

    /* Squatted directory (same uid, wrong mode): startup fails closed,
     * names the path, and creates nothing inside. */
    $lockfilePath = $root . '/lock-mode';
    mkdir($lockfilePath . '/' . $privateName, 0755, true);

    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=seed');
    echo "wrong-mode: $stdout\n";
    echo str_replace($root, '%ROOT%', $stderr), "\n";
    var_dump(user_cache_private_dir_entries($lockfilePath . '/' . $privateName));

    /* Symlink planted at the directory name: never followed. */
    $lockfilePath = $root . '/lock-symlink';
    mkdir($lockfilePath, 0777);
    mkdir($root . '/symlink-target', 0700);
    symlink($root . '/symlink-target', $lockfilePath . '/' . $privateName);

    [$stdout, $stderr] = user_cache_cgi_run($phpCgi, $script, $docRoot, $lockfilePath, 'action=seed');
    echo "symlink: $stdout\n";
    echo str_replace($root, '%ROOT%', $stderr), "\n";
    var_dump(user_cache_private_dir_entries($root . '/symlink-target'));

    echo "Done\n";
} finally {
    user_cache_cgi_rm_rf($root);
}

?>
--EXPECTF--
first: Available:seeded
second: Available:seeded
bool(true)
bool(true)
string(4) "0700"
int(2)
int(32)
string(4) "0600"
bool(true)
string(4) "0600"
bool(true)
wrong-mode: UnavailableBySharedMemoryInitializationFailed:MISS
UserCache boundary directory %ROOT%/lock-mode/.ZendUserCacheBnd.%d is unusable (not a private directory owned by this uid); it must be a directory owned by uid %d with mode 0700 (see user_cache.lockfile_path)
UserCache partition startup failed; UserCache will be unavailable
array(0) {
}
symlink: UnavailableBySharedMemoryInitializationFailed:MISS
UserCache boundary directory %ROOT%/lock-symlink/.ZendUserCacheBnd.%d is unusable (%s); it must be a directory owned by uid %d with mode 0700 (see user_cache.lockfile_path)
UserCache partition startup failed; UserCache will be unavailable
array(0) {
}
Done
