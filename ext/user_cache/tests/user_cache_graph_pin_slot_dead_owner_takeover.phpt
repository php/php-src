--TEST--
UserCache\Cache: once every graph pin and reader slot belongs to a dead process, the next reader takes one over and still gets a pinned view
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!PHP_DEBUG) {
    die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
}
if (getenv('USE_ZEND_ALLOC') === '0') {
    die('skip forks 256 processes under a memory checker');
}
?>
--ENV--
USER_CACHE_DEBUG_EXIT_BEFORE_CLAIM_RELEASE=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
const GENERATIONS = 256;

$cache = UserCache\Cache::getPool('graph-pin-slot-takeover');
$cache->clear();

$value = ['list' => range(1, 16), 'text' => str_repeat('t', 200), 'nested' => ['k' => ['v' => 1.5]]];
$cache->store('graph', $value);

/* Each short-lived process claims a pin slot and a reader slot, then dies before it gives them back */
$failures = 0;
for ($i = 0; $i < GENERATIONS; $i++) {
    $pid = pcntl_fork();
    if ($pid < 0) die("fork failed\n");
    if ($pid === 0) {
        $held = UserCache\Cache::getPool('graph-pin-slot-takeover')->fetch('graph');
        exit($held === $value ? 0 : 1);
    }
    pcntl_waitpid($pid, $status);
    if (!pcntl_wifexited($status) || pcntl_wexitstatus($status) !== 0) {
        $failures++;
    }
}
var_dump($failures);

$status = UserCache\Cache::getStatus();
var_dump($status->getGraphPinSlotsInUse(), $status->getGraphPinnedReferences());

/* This process never claimed a slot: it must reclaim one from an exited owner */
$fetched = $cache->fetch('graph');
var_dump($fetched === $value);
$status = UserCache\Cache::getStatus();
var_dump($status->getGraphPinSlotsInUse(), $status->getGraphPinnedReferences());

/* The pinned view stays valid after the entry is deleted and is released with it */
var_dump($cache->delete('graph'));
var_dump($fetched === $value);
unset($fetched);
var_dump($cache->store('graph', $value), $cache->fetch('graph') === $value);
?>
--EXPECT--
int(0)
int(256)
int(0)
bool(true)
int(256)
int(1)
bool(true)
bool(true)
bool(true)
bool(true)
