--TEST--
UserCache\Cache: a writer that dies inside a write section is recovered by the next writer, keeping live entry locks
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=8M
--FILE--
<?php
use UserCache\Cache;

$cache = Cache::getPool('recovery-writer');
$cache->clear();
for ($i = 0; $i < 20; $i++) {
    $cache->store("k$i", str_repeat('v', 500) . $i);
}
var_dump($cache->lock('held', 60));

$pid = pcntl_fork();
if ($pid === 0) {
    putenv('USER_CACHE_DEBUG_EXIT_IN_WRITE_SECTION=1');
    $cache->store('child', 'never published');
    exit(1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);

/* The next writer wipes the entries the dead writer may have torn. */
var_dump($cache->store('after', 'ok'));
var_dump(Cache::getStatus()->getAvailability() === UserCache\CacheAvailability::Available);
var_dump($cache->fetch('k0'), $cache->fetch('child'), $cache->fetch('after'), Cache::getStatus()->getEntryCount());

/* A lock held by a live process survives the wipe. */
$pid = pcntl_fork();
if ($pid === 0) {
    exit($cache->lock('held') ? 1 : 0);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wexitstatus($status) === 0);
var_dump($cache->unlock('held'));
var_dump($cache->store('k0', 'again'), $cache->fetch('k0'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
NULL
NULL
string(2) "ok"
int(1)
bool(true)
bool(true)
bool(true)
string(5) "again"
