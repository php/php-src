--TEST--
UserCache\Cache: a process killed while it takes over a dead owner's graph pin slot leaves the slot reclaimable
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
if (getenv('USE_ZEND_ALLOC') === '0') die('skip forks 512 processes under a memory checker');
?>
--EXTENSIONS--
pcntl
--ENV--
USER_CACHE_DEBUG_EXIT_BEFORE_CLAIM_RELEASE=1
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
const PIN_SLOTS = 256;

function fork_each(int $count, callable $body): void
{
    for ($i = 0; $i < $count; $i++) {
        $pid = pcntl_fork();
        if ($pid === 0) {
            $body();
            exit(0);
        }
        pcntl_waitpid($pid, $status);
    }
}

$cache = UserCache\Cache::getPool('pin-slot-reclaim-exit');
$cache->clear();
$cache->store('value', range(1, 8));

/* Every child dies after its request released its pins and before it gives back its slot. */
fork_each(PIN_SLOTS, function () {
    UserCache\Cache::getPool('pin-slot-reclaim-exit')->fetch('value');
});
var_dump(UserCache\Cache::getStatus()->getGraphPinSlotsInUse());

/* Every child dies after it took a dead owner's slot and before it recorded its own start time. */
fork_each(PIN_SLOTS, function () {
    putenv('USER_CACHE_DEBUG_EXIT_IN_GRAPH_PIN_SLOT_RECLAIM=1');
    UserCache\Cache::getPool('pin-slot-reclaim-exit')->fetch('value');
});

/* The slots are still reclaimable: this fetch pins the payload instead of decoding a private copy. */
var_dump($cache->fetch('value') === range(1, 8));
var_dump(UserCache\Cache::getStatus()->getGraphPinnedReferences());
?>
--EXPECT--
int(256)
bool(true)
int(1)
