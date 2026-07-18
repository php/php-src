--TEST--
UserCache\Cache: TTL and remember()
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('ttl-remember');
var_dump($cache->store('ttl', 'value', 1));
var_dump($cache->has('ttl'));
sleep(2);
var_dump($cache->fetch('ttl', 'expired'));

$calls = 0;
var_dump($cache->remember('remembered', function () use (&$calls) {
    $calls++;
    return 'computed';
}));
var_dump($cache->remember('remembered', function () use (&$calls) {
    $calls++;
    return 'ignored';
}));
var_dump($calls);
?>
--EXPECT--
bool(true)
bool(true)
string(7) "expired"
string(8) "computed"
string(8) "computed"
int(1)
