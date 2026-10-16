--TEST--
CGI/FastCGI: a process that finds the lock file of an fcntl-locked boundary segment replaced retires the segment name without writing to the old segment, so the old and new processes use separate segments
--CONFLICTS--
all
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
if (PHP_OS_FAMILY !== 'Linux') die('skip names boundary segments under /dev/shm');
if (!function_exists('proc_open')) die('skip proc_open() not available');
require_once __DIR__ . '/user_cache_fcgi_tester.inc';
if (FcgiTester::binary() === null) die('skip CGI SAPI binary not available');
?>
--FILE--
<?php

require_once __DIR__ . '/user_cache_fcgi_tester.inc';

$segments = fn(): array => glob('/dev/shm/PUC.*') ?: [];
$before = $segments();
$old = new FcgiTester('lockfile-replaced-old');
$new = new FcgiTester('lockfile-replaced-new');

try {
    $root = $old->docRoot('site', <<<'PHP'
<?php
$cache = UserCache\Cache::getPool('lockfile-replaced');
if (isset($_GET['value'])) {
    $cache->store('key', $_GET['value']);
}
echo $cache->fetch('key', 'MISS'), ' ', UserCache\Cache::getStatus()->getAvailability()->name;
PHP);
    file_put_contents($root . '/ready.php', '<?php echo "ready";');
    $script = $root . '/index.php';
    $locks = $old->path('locks');
    mkdir($locks);
    $ini = ['user_cache.enable=1', 'user_cache.shm_size=8M', 'user_cache.lockfile_path=' . $locks];

    putenv('USER_CACHE_DEBUG_FORCE_FCNTL_LOCK_MODEL=1');

    $old->start($ini, $root . '/ready.php', $root, 'lockfile.local', '');
    echo 'old: ', $old->request($script, $root, 'lockfile.local', 'value=old'), "\n";
    $created = array_values(array_diff($segments(), $before));
    var_dump(count($created));

    /* The scalar header fields (sequence, counts, allocator state) of the segment the old process uses; the name may be reused later. */
    $oldSegment = fopen($created[0], 'rb');
    $oldHeader = function () use ($oldSegment): string {
        rewind($oldSegment);

        return fread($oldSegment, 176);
    };
    $oldHeaderBefore = $oldHeader();

    /* A temp cleaner deletes the lock file while the old process keeps its descriptor. */
    foreach (glob($locks . '/.PhpUserCacheBnd.*/*.lock') as $lock) {
        unlink($lock);
    }

    $new->start($ini, $root . '/ready.php', $root, 'lockfile.local', '');
    echo 'new: ', $new->request($script, $root, 'lockfile.local', ''), "\n";
    var_dump($oldHeader() === $oldHeaderBefore);
    echo 'new: ', $new->request($script, $root, 'lockfile.local', 'value=new'), "\n";
    echo 'old: ', $old->request($script, $root, 'lockfile.local', ''), "\n";
    echo 'new: ', $new->request($script, $root, 'lockfile.local', ''), "\n";
    var_dump(count(array_diff($segments(), $before)));

    putenv('USER_CACHE_DEBUG_FORCE_FCNTL_LOCK_MODEL');
} finally {
    if (isset($oldSegment)) {
        fclose($oldSegment);
    }
    $new->stop();
    $old->stop();
    FcgiTester::removeBoundarySegments($locks ?? $old->path('locks'));
    foreach (array_diff($segments(), $before) as $segment) {
        @unlink($segment);
    }
    $new->cleanup();
    $old->cleanup();
}
?>
--EXPECT--
old: old Available
int(1)
new: MISS Available
bool(true)
new: new Available
old: old Available
new: new Available
int(1)
