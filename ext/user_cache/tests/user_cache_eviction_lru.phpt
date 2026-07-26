--TEST--
UserCache\Cache: LRU eviction under memory pressure keeps recent entries and never wipes
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=4M
--FILE--
<?php
$cache = UserCache\Cache::getPool('lru');
$blob = str_repeat('x', 8192);

/* Fill the data region completely (no pressure yet). */
$stored = 0;
for ($i = 0; $i < 476; $i++) {
    if (!$cache->store('k' . $i, $blob . $i)) {
        break;
    }
    $stored++;
}
var_dump($stored > 400);

/* Advance the coarse access clock to the next second; the TTL'd store
 * refreshes the per-request clock memo through its real time() read. */
sleep(1);
$cache->store('clock', 'tick', 60);

/* Re-stamp a slice of old keys. */
for ($i = 0; $i < 10; $i++) {
    $cache->fetch('k' . $i);
}

/* Force eviction pressure. */
$pressured_ok = 0;
for ($i = 0; $i < 30; $i++) {
    if ($cache->store('new' . $i, $blob . 'n' . $i)) {
        $pressured_ok++;
    }
}
var_dump($pressured_ok === 30);

$touched_alive = 0;
for ($i = 0; $i < 10; $i++) {
    if ($cache->fetch('k' . $i) !== null) {
        $touched_alive++;
    }
}
$new_alive = 0;
for ($i = 0; $i < 30; $i++) {
    if ($cache->fetch('new' . $i) !== null) {
        $new_alive++;
    }
}

$status = UserCache\Cache::getStatus();
var_dump($touched_alive === 10);
var_dump($new_alive === 30);
/* Cache stays nearly full: eviction is targeted, not a wipe. */
var_dump($status->getEntryCount() > 400);
var_dump($status->getEvictionCount() > 0);
var_dump($status->getExpungeCount() === 0);
var_dump($status->getStoreFailureCount() === 0);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
