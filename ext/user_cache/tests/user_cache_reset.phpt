--TEST--
UserCache\Cache: user_cache_reset() clears cached values in the active partition
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('reset-a');
$other = UserCache\Cache::getPool('reset-b');

var_dump($cache->store('key', ['value' => 1]));
var_dump($other->store('key', 'other'));

var_dump($cache->fetch('key'));
var_dump($other->fetch('key'));

var_dump(user_cache_reset());

var_dump($cache->fetch('key', 'missing'));
var_dump($other->fetch('key', 'missing'));
var_dump($cache->has('key'));
var_dump($other->has('key'));
?>
--EXPECT--
bool(true)
bool(true)
array(1) {
  ["value"]=>
  int(1)
}
string(5) "other"
bool(true)
string(7) "missing"
string(7) "missing"
bool(false)
bool(false)
