--TEST--
UserCache\Cache: recovery after a writer died inside a write section releases the graph pins of the dead process
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

$cache = Cache::getPool('recovery-dead-reader-pins');
for ($i = 0; $i < 10; $i++) {
    $cache->store("k$i", range(0, 50 + $i));
}

$reclaimed = Cache::getStatus()->getDeadPinOwnersReclaimed();

$pid = pcntl_fork();
if ($pid === 0) {
    $held = [];
    for ($i = 0; $i < 10; $i++) {
        $held[] = $cache->fetch("k$i");
    }
    putenv('USER_CACHE_DEBUG_EXIT_IN_WRITE_SECTION=1');
    $cache->store('child', 'never published');
    exit(1);
}
pcntl_waitpid($pid, $status);
var_dump(pcntl_wifexited($status) && pcntl_wexitstatus($status) === 0);

var_dump($cache->store('after', 'ok'));

$status = Cache::getStatus();
var_dump($status->getGraphPinnedReferences());
var_dump($status->getGraphPinSlotsInUse());
var_dump($status->getDeadPinOwnersReclaimed() - $reclaimed);
var_dump($cache->fetch('after'));
?>
--EXPECT--
bool(true)
bool(true)
int(0)
int(0)
int(1)
string(2) "ok"
