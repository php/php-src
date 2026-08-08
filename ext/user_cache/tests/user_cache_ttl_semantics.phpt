--TEST--
UserCache\Cache: counter TTL expiry and TTL overwrite semantics
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('ttl-semantics');
$cache->clear();

var_dump($cache->increment('counter', 1, 1));

var_dump($cache->store('overwritten', 'v1', 1));
var_dump($cache->store('overwritten', 'v2'));

var_dump($cache->store('shortened', 'v1'));
var_dump($cache->store('shortened', 'v2', 1));

sleep(2);

var_dump($cache->fetch('counter', 'MISS'));
var_dump($cache->increment('counter'));
var_dump($cache->fetch('overwritten', 'MISS'));
var_dump($cache->fetch('shortened', 'MISS'));
?>
--EXPECT--
int(1)
bool(true)
bool(true)
bool(true)
bool(true)
string(4) "MISS"
int(1)
string(2) "v2"
string(4) "MISS"
