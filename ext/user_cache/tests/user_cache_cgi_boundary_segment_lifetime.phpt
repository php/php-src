--TEST--
CGI: idle boundary segments outlive php-cgi restarts, and a process that attaches a boundary removes the idle segments of the same boundary with another layout, orphaned lock files and the least recently used idle segments beyond 32
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip names boundary segments under /dev/shm');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require_once __DIR__ . '/user_cache_fcgi_tester.inc';
if (FcgiTester::binary() === null) die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

$root = sys_get_temp_dir() . '/php-user-cache-cgi-segment-lifetime-' . getmypid();
$site = $root . '/site';
$locks = $root . '/locks';
$capLocks = $root . '/cap-locks';
@mkdir($site, 0777, true);
@mkdir($locks, 0777, true);
@mkdir($capLocks, 0777, true);

$script = <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('segment-lifetime');
if (isset($_GET['value'])) {
    $cache->store('k', $_GET['value']);
}
if (isset($_GET['release'])) {
    if (pcntl_fork() === 0) {
        while (!file_exists($_GET['release'])) {
            usleep(10000);
        }
        exit(0);
    }
}
echo $cache->fetch('k', 'MISS');
PHP;
file_put_contents($site . '/index.php', $script);

function cgi(string $docRoot, string $lockPath, string $shmSize, string $query = ''): string
{
    $out = $docRoot . '/../out.txt';
    $process = proc_open(
        [FcgiTester::binary(), '-q', '-n', '-d', 'user_cache.lockfile_path=' . $lockPath, '-d', 'user_cache.shm_size=' . $shmSize],
        [['pipe', 'r'], ['file', $out, 'w'], ['file', $out . '.err', 'w']],
        $pipes,
        $docRoot,
        [
            'REDIRECT_STATUS' => '1',
            'REQUEST_METHOD' => 'GET',
            'SCRIPT_FILENAME' => $docRoot . '/index.php',
            'DOCUMENT_ROOT' => $docRoot,
            'SERVER_NAME' => 'segment-lifetime.local',
            'QUERY_STRING' => $query,
        ]
    );
    fclose($pipes[0]);
    proc_close($process);

    $response = explode("\r\n\r\n", file_get_contents($out), 2);

    return trim(end($response) . file_get_contents($out . '.err'));
}

function lock_ids(string $lockPath): array
{
    return array_map(fn($lock) => basename($lock, '.lock'), glob($lockPath . '/.PhpUserCacheBnd.*/*.lock') ?: []);
}

function segments_exist(array $ids): bool
{
    foreach ($ids as $id) {
        if (!is_file('/dev/shm/PUC.' . $id)) {
            return false;
        }
    }

    return true;
}

function wait_for_lock_count(string $lockPath, int $count): int
{
    for ($i = 0; $i < 500 && count(lock_ids($lockPath)) !== $count; $i++) {
        usleep(10000);
    }

    return count(lock_ids($lockPath));
}

try {
    echo "restart:\n";
    echo cgi($site, $locks, '1M', 'value=first'), "\n";
    echo cgi($site, $locks, '1M'), "\n";
    $first = lock_ids($locks);
    var_dump(count($first), segments_exist($first));

    echo "other layout replaces the idle segment:\n";
    echo cgi($site, $locks, '2M'), "\n";
    $second = lock_ids($locks);
    var_dump(count($second), $second !== $first, segments_exist($second), is_file('/dev/shm/PUC.' . $first[0]));
    echo cgi($site, $locks, '1M'), "\n";
    var_dump(count(lock_ids($locks)), is_file('/dev/shm/PUC.' . $second[0]));

    echo "orphaned lock file:\n";
    $private = dirname(glob($locks . '/.PhpUserCacheBnd.*/*.lock')[0]);
    touch($private . '/000000000000000000000000.lock');
    chmod($private . '/000000000000000000000000.lock', 0600);
    touch($private . '/not-a-lock-file.lock');
    echo cgi($site, $locks, '1M', 'value=second'), "\n";
    var_dump(file_exists($private . '/000000000000000000000000.lock'), file_exists($private . '/not-a-lock-file.lock'));
    unlink($private . '/not-a-lock-file.lock');

    echo "a segment in use by a forked child is kept until the child detaches:\n";
    $release = $root . '/release';
    echo cgi($site, $locks, '1M', 'value=forked&release=' . urlencode($release)), "\n";
    $held = lock_ids($locks);
    echo cgi($site, $locks, '2M'), "\n";
    $both = lock_ids($locks);
    var_dump(count($both), in_array($held[0], $both, true), segments_exist($both));
    touch($release);
    var_dump(wait_for_lock_count($locks, 1), in_array($held[0], lock_ids($locks), true), segments_exist(lock_ids($locks)));

    echo "idle segments beyond the limit:\n";
    for ($i = 1; $i <= 34; $i++) {
        @mkdir($root . "/sites/s$i", 0777, true);
        file_put_contents($root . "/sites/s$i/index.php", $script);
        $out = cgi($root . "/sites/s$i", $capLocks, '1M', "value=s$i");
        if ($out !== "s$i") {
            echo "site $i: $out\n";
        }
    }
    $capped = lock_ids($capLocks);
    var_dump(count($capped), segments_exist($capped));
    echo cgi($root . '/sites/s1', $capLocks, '1M'), "\n";
    echo cgi($root . '/sites/s34', $capLocks, '1M'), "\n";
    echo cgi($root . '/sites/s3', $capLocks, '1M'), "\n";
    echo cgi($root . '/sites/s2', $capLocks, '1M'), "\n";
    var_dump(count(lock_ids($capLocks)), segments_exist(lock_ids($capLocks)));
} finally {
    @touch($root . '/release');
    wait_for_lock_count($locks, 1);
    FcgiTester::removeBoundarySegments($root);
    $paths = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($root, FilesystemIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($paths as $path) {
        $path->isDir() && !$path->isLink() ? rmdir($path->getPathname()) : unlink($path->getPathname());
    }
    rmdir($root);
}
?>
--EXPECT--
restart:
first
first
int(1)
bool(true)
other layout replaces the idle segment:
MISS
int(1)
bool(true)
bool(true)
bool(false)
MISS
int(1)
bool(false)
orphaned lock file:
second
bool(false)
bool(true)
a segment in use by a forked child is kept until the child detaches:
forked
MISS
int(2)
bool(true)
bool(true)
int(1)
bool(false)
bool(true)
idle segments beyond the limit:
int(33)
bool(true)
MISS
s34
s3
MISS
int(33)
bool(true)
