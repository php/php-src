--TEST--
UserCache\Cache: basic API
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('basic');
$status = UserCache\Cache::getStatus();
$poolStatus = $cache->getPoolStatus();
var_dump($poolStatus->getPoolName(), $status->getAvailability()->name, $poolStatus->getEntryCount());

var_dump($cache->store('array', ['x' => 1]));
var_dump($cache->fetch('array'));
var_dump($cache->has('array'));

$fetched = $cache->fetch('array');
$fetched['x'] = 2;
var_dump($cache->fetch('array'));

var_dump($cache->storeMultiple(['one' => 1, 'two' => 'x'], 60));
var_dump($cache->fetch('one'));
var_dump($cache->fetchMultiple(['one', 'missing'], 'fallback'));
var_dump($cache->fetch('missing', 'fallback'));

var_dump($cache->delete('one'));
var_dump($cache->has('one'));

var_dump($cache->delete('never-stored'));
var_dump($cache->deleteMultiple(['never-stored', 'also-missing']));

var_dump($cache->lock('lock'));
var_dump($cache->unlock('lock'));

var_dump($cache->lock('locked-store'));
var_dump($cache->store('locked-store', 'value'));
var_dump($cache->unlock('locked-store'));

var_dump($cache->lock('locked-delete'));
var_dump($cache->store('locked-delete', 'value'));
var_dump($cache->delete('locked-delete'));
var_dump($cache->unlock('locked-delete'));

var_dump($cache->lock('locked-clear'));
var_dump($cache->clear());
var_dump($cache->unlock('locked-clear'));
var_dump($cache->has('two'));
?>
--EXPECT--
string(5) "basic"
string(9) "Available"
int(0)
bool(true)
array(1) {
  ["x"]=>
  int(1)
}
bool(true)
array(1) {
  ["x"]=>
  int(1)
}
bool(true)
int(1)
array(2) {
  ["one"]=>
  int(1)
  ["missing"]=>
  string(8) "fallback"
}
string(8) "fallback"
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
