--TEST--
UserCache\Cache: expired entries observed by reads are reclaimed by a later mutation
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
; Capacity fits inside one bounded expunge window, so a single mutation
; reclaims every observed expired entry.
user_cache.shm_size=1M
--FILE--
<?php
$cache = UserCache\Cache::getPool('expired-read-reclaim');

for ($i = 0; $i < 80; $i++) {
    $cache->store('expiring-' . $i, str_repeat('v', 64), 1);
}
$cache->store('persistent', 'kept');

var_dump($cache->getPoolStatus()->getEntryCount());

sleep(2);

/* Cross the observation threshold before triggering expiry cleanup. */
for ($i = 0; $i < 80; $i++) {
    if ($cache->fetch('expiring-' . $i, null) !== null) {
        exit('unexpected hit');
    }
}

$cache->store('trigger', 'mutation');

var_dump($cache->getPoolStatus()->getEntryCount());
var_dump($cache->fetch('persistent'));
var_dump($cache->fetch('trigger'));
?>
--EXPECT--
int(81)
int(2)
string(4) "kept"
string(8) "mutation"
