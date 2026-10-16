--TEST--
UserCache\Cache: a process that attaches to a boundary segment whose creator died before reserving it reserves the pages itself
--CONFLICTS--
all
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
if (PHP_OS_FAMILY !== 'Linux') die('skip needs tmpfs block accounting of /dev/shm');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require_once __DIR__ . '/user_cache_fcgi_tester.inc';
if (FcgiTester::binary() === null) die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

$segments = fn(): array => glob('/dev/shm/PUC.*') ?: [];
$before = $segments();
$server = new FcgiTester('sparse-segment');

try {
    $root = $server->docRoot('site', <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('sparse-segment');
$cache->store('k', 'stored');
echo $cache->fetch('k', 'MISS');
PHP);
    $script = $root . '/index.php';
    $ini = ['user_cache.enable=1', 'user_cache.shm_size=8M'];

    putenv('USER_CACHE_DEBUG_EXIT_IN_BOUNDARY_SEGMENT_CREATE=1');
    try {
        $server->start($ini, $script, $root, 'sparse.local', '');
        echo "creator survived\n";
    } catch (RuntimeException) {
        echo "creator died after sizing the segment\n";
    }
    $server->stop();
    putenv('USER_CACHE_DEBUG_EXIT_IN_BOUNDARY_SEGMENT_CREATE');

    $created = array_values(array_diff($segments(), $before));
    var_dump(count($created));
    clearstatcache();
    $st = stat($created[0]);
    echo "left sparse: ";
    var_dump($st['size'] === 8 << 20 && $st['blocks'] * 512 < $st['size']);

    $server->start($ini, $script, $root, 'sparse.local', '');
    echo $server->request($script, $root, 'sparse.local', ''), "\n";
    $server->stop();

    clearstatcache();
    $st = stat($created[0]);
    echo "reserved by the next process: ";
    var_dump($st['blocks'] * 512 >= $st['size']);
} finally {
    $server->cleanup();
    foreach (array_diff($segments(), $before) as $segment) {
        @unlink($segment);
    }
}
?>
--EXPECT--
creator died after sizing the segment
int(1)
left sparse: bool(true)
stored
reserved by the next process: bool(true)
