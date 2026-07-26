--TEST--
UserCache\Cache: LRU eviction skips entries whose key is under an active lock
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=4M
--FILE--
<?php
$cache = UserCache\Cache::getPool('lockskip');
$blob = str_repeat('x', 8192);

for ($i = 0; $i < 476; $i++) {
    if (!$cache->store('k' . $i, $blob . $i)) {
        break;
    }
}

/* Hold a per-key lock on one cold entry. */
var_dump($cache->lock('k3'));

/* Advance the coarse access clock, then churn far past the old cohort so
 * every unlocked old entry becomes an eviction victim. */
sleep(1);
$cache->store('clock', 'tick', 60);
for ($i = 0; $i < 600; $i++) {
    $cache->store('new' . $i, $blob . 'n' . $i);
}

/* The locked key survived the full churn; its unlocked neighbors did not. */
var_dump($cache->fetch('k3') !== null);
var_dump($cache->fetch('k2', 'MISS') === 'MISS');
var_dump($cache->fetch('k4', 'MISS') === 'MISS');
var_dump(UserCache\Cache::getStatus()->getExpungeCount() === 0);

var_dump($cache->unlock('k3'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
