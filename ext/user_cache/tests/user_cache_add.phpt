--TEST--
UserCache\Cache: add() stores only when the key is absent
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('add');
$cache->clear();

var_dump($cache->add('key', 'first'));
var_dump($cache->add('key', 'second'));
var_dump($cache->fetch('key'));

var_dump($cache->store('key', 'overwritten'));
var_dump($cache->fetch('key'));

var_dump($cache->delete('key'));
var_dump($cache->add('key', 'after-delete'));
var_dump($cache->fetch('key'));

var_dump($cache->add('ttl-key', 'v', 60));
var_dump($cache->fetch('ttl-key'));
?>
--EXPECT--
bool(true)
bool(false)
string(5) "first"
bool(true)
string(11) "overwritten"
bool(true)
bool(true)
string(12) "after-delete"
bool(true)
string(1) "v"
