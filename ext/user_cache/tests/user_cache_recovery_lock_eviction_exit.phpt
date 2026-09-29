--TEST--
UserCache\Cache: recovery after lock() died while evicting for its key runs only once
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--EXTENSIONS--
pcntl
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=1M
user_cache.eviction_policy=lru
--FILE--
<?php
$cache = UserCache\Cache::getPool('recovery-lock-eviction');
$cache->clear();
for ($i = 0; $i < 400; $i++) {
    $cache->store("fill-$i", str_repeat('x', 4000));
}
$key = str_repeat('k', UserCache\Cache::getStatus()->getFreeMemory() + 1000);

/* The lock-table section turned into a write section when its process died. */
$pid = pcntl_fork();
if ($pid === 0) {
    putenv('USER_CACHE_DEBUG_EXIT_IN_ENTRY_LOCK_EVICTION=1');
    $cache->lock($key);
    exit(1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);

var_dump($cache->store('b', 'first after recovery'));
var_dump($cache->store('c', 'second'));
var_dump($cache->fetch('b'), $cache->fetch('c'));
var_dump($cache->lock($key), $cache->unlock($key));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(20) "first after recovery"
string(6) "second"
bool(true)
bool(true)
