--TEST--
UserCache\Cache: entry locks are isolated by exact key and re-entrant only for their owner
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
/* A held lock never blocks operations on other keys, including prefix-sharing ones. */
$cache = UserCache\Cache::getPool('locks-key-isolation');
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

/* The owner may re-lock its key; unlocking an unheld key fails. */
$cache = UserCache\Cache::getPool('locks-reentrancy');

var_dump($cache->lock('key'));
var_dump($cache->lock('key'));
var_dump($cache->unlock('key'));
var_dump($cache->unlock('key'));

var_dump($cache->unlock('never-locked'));
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
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
