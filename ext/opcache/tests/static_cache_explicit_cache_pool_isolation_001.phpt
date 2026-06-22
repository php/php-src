--TEST--
OPcache explicit cache pools isolate keys, clear, reset, and fetchMultiple result keys
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

opcache_static_cache_volatile_reset();

$volatileA = OPcache\VolatileCache::getInstance('isolation_a');
$volatileB = OPcache\VolatileCache::getInstance('isolation_b');
$stableA = OPcache\StableCache::getInstance('isolation_a');
$stableB = OPcache\StableCache::getInstance('isolation_b');
$stableA->clear();
$stableB->clear();

$volatileA->store('key', 'volatile-a');
$volatileB->store('key', 'volatile-b');
$stableA->store('key', 'stable-a');
$stableB->store('key', 'stable-b');

var_dump($volatileA->fetch('key'));
var_dump($volatileB->fetch('key'));
var_dump($stableA->fetch('key'));
var_dump($stableB->fetch('key'));

var_dump($volatileA->fetchMultiple(['key', 'missing'], ['fallback']));
var_dump($stableA->fetchMultiple(['key', 'missing'], ['fallback']));

$volatileA->clear();
$stableA->clear();
var_dump($volatileA->fetch('key', 'missing'));
var_dump($volatileB->fetch('key', 'missing'));
var_dump($stableA->fetch('key', 'missing'));
var_dump($stableB->fetch('key', 'missing'));

$volatileA->lock('pool_lock');
$volatileB->lock('pool_lock');
$stableA->lock('pool_lock');
$stableB->lock('pool_lock');
$volatileA->clear();
$stableA->clear();
var_dump($volatileA->unlock('pool_lock'));
var_dump($volatileB->unlock('pool_lock'));
var_dump($stableA->unlock('pool_lock'));
var_dump($stableB->unlock('pool_lock'));

$volatileA->store('reset', 'volatile-a');
$volatileB->store('reset', 'volatile-b');
$stableB->store('reset', 'stable-b');
$volatileA->lock('reset_lock');
$volatileB->lock('reset_lock');
$stableB->lock('reset_lock');
opcache_static_cache_volatile_reset();
var_dump($volatileA->unlock('reset_lock'));
var_dump($volatileB->unlock('reset_lock'));
var_dump($stableB->unlock('reset_lock'));
var_dump($volatileA->fetch('reset', 'missing'));
var_dump($volatileB->fetch('reset', 'missing'));
var_dump($stableB->fetch('reset', 'missing'));

?>
--EXPECT--
string(10) "volatile-a"
string(10) "volatile-b"
string(8) "stable-a"
string(8) "stable-b"
array(2) {
  ["key"]=>
  string(10) "volatile-a"
  ["missing"]=>
  array(1) {
    [0]=>
    string(8) "fallback"
  }
}
array(2) {
  ["key"]=>
  string(8) "stable-a"
  ["missing"]=>
  array(1) {
    [0]=>
    string(8) "fallback"
  }
}
string(7) "missing"
string(10) "volatile-b"
string(7) "missing"
string(8) "stable-b"
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
string(7) "missing"
string(7) "missing"
string(8) "stable-b"
