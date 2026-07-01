--TEST--
OPcache User Cache: TTL and remember()
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--FILE--
<?php
opcache_user_cache_reset();

$cache = new Opcache\UserCache('ttl-remember');
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
