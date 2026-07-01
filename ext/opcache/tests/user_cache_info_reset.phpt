--TEST--
OPcache User Cache: info statistics and user cache reset
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=16M
--FILE--
<?php
$cache = new Opcache\UserCache('info-reset-a');
$other = new Opcache\UserCache('info-reset-b');
$cache->clear();
$other->clear();

$initial = $cache->info();
var_dump($initial->scope);
var_dump($initial->available);
var_dump($initial->enabled);
var_dump($initial->startupFailed);
var_dump($initial->backendInitialized);
var_dump($initial->configuredMemory === 16 * 1024 * 1024);
var_dump($initial->sharedMemorySize > 0);
var_dump($initial->entryCount);

var_dump($cache->store('key', ['value' => 1]));
var_dump($other->store('key', 'other'));

$afterStore = $cache->info();
var_dump($afterStore->entryCount >= 2);
var_dump($afterStore->entryCapacity > 0);
var_dump($afterStore->usedMemory > 0);
var_dump($afterStore->freeMemory > 0);
var_dump($afterStore->wastedMemory >= 0);
var_dump($afterStore->tombstoneCount >= 0);

var_dump(opcache_user_cache_reset());
var_dump($cache->fetch('key', 'missing'));
var_dump($other->fetch('key', 'missing'));

$afterReset = $cache->info();
var_dump($afterReset->entryCount);
?>
--EXPECT--
string(12) "info-reset-a"
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
int(0)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(7) "missing"
string(7) "missing"
int(0)
