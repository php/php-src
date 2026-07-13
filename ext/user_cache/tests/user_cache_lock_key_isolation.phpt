--TEST--
UserCache\Cache: locks are isolated by exact key
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('lock-key-isolation');
$cache->clear();

var_dump($cache->lock('alpha'));
var_dump($cache->store('beta', 'beta-value'));
var_dump($cache->fetch('beta'));
var_dump($cache->delete('beta'));
var_dump($cache->has('beta'));

var_dump($cache->storeMultiple([
    'gamma' => 'gamma-value',
    'delta' => 'delta-value',
]));
var_dump($cache->fetchMultiple(['gamma', 'delta']));
var_dump($cache->deleteMultiple(['gamma', 'delta']));
var_dump($cache->has('gamma'));
var_dump($cache->has('delta'));

var_dump($cache->lock('epsilon'));
var_dump($cache->unlock('epsilon'));
var_dump($cache->unlock('alpha'));

var_dump($cache->lock('prefix'));
var_dump($cache->store('prefix:child', 'child-value'));
var_dump($cache->fetch('prefix:child'));
var_dump($cache->unlock('prefix'));
?>
--EXPECT--
bool(true)
bool(true)
string(10) "beta-value"
bool(true)
bool(false)
bool(true)
array(2) {
  ["gamma"]=>
  string(11) "gamma-value"
  ["delta"]=>
  string(11) "delta-value"
}
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(11) "child-value"
bool(true)
