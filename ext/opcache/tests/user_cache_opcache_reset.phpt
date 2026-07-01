--TEST--
OPcache User Cache: opcache_reset clears cached values
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--FILE--
<?php
$cache = new Opcache\UserCache('reset-a');
$other = new Opcache\UserCache('reset-b');

var_dump($cache->store('key', ['value' => 1]));
var_dump($other->store('key', 'other'));

var_dump($cache->fetch('key'));
var_dump($other->fetch('key'));

var_dump(opcache_reset());

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
