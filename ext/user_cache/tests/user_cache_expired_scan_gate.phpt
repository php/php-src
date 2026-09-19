--TEST--
UserCache\Cache: the periodic expired-entry scan still reclaims deadlines after TTL-free writes
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('expired-scan');
$status = static fn (): UserCache\CacheStatus => UserCache\Cache::getStatus();
$cache->clear();

for ($i = 0; $i < 8; $i++) {
    var_dump($cache->store('ttl-' . $i, $i, 1));
}
for ($i = 0; $i < 8; $i++) {
    $cache->store('keep-' . $i, $i);
}
var_dump($status()->getEntryCount());

sleep(2);

/* Overwrites of TTL-free keys never touch the deadlines, yet enough of them
 * must still drive the bounded scan over the expired entries. */
for ($i = 0; $i < 256; $i++) {
    $cache->store('keep-' . ($i % 8), $i);
}
var_dump($status()->getEntryCount());

for ($i = 0; $i < 8; $i++) {
    var_dump($cache->fetch('ttl-' . $i, 'MISS'));
}

/* Deadlines that are replaced by TTL-free values leave nothing to scan. */
var_dump($cache->store('ttl-again', 'v1', 1));
var_dump($cache->store('ttl-again', 'v2'));
sleep(2);
for ($i = 0; $i < 256; $i++) {
    $cache->store('keep-' . ($i % 8), $i);
}
var_dump($cache->fetch('ttl-again', 'MISS'));
var_dump($status()->getEntryCount());
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
int(16)
int(8)
string(4) "MISS"
string(4) "MISS"
string(4) "MISS"
string(4) "MISS"
string(4) "MISS"
string(4) "MISS"
string(4) "MISS"
string(4) "MISS"
bool(true)
bool(true)
string(2) "v2"
int(9)
