--TEST--
OPcache User Cache: storeMultiple rolls back partial writes on failure
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
opcache.user_cache_shm_size=1M
--FILE--
<?php
opcache_user_cache_reset();

$cache = new Opcache\UserCache('store-multiple-rollback');

var_dump($cache->store('existing-a', 'old-a'));
var_dump($cache->store('existing-b', 'old-b'));

var_dump($cache->storeMultiple([
    'existing-a' => 'new-a',
    'new-key' => 'new-value',
    'existing-b' => str_repeat('x', 4 * 1024 * 1024),
]));

var_dump($cache->fetch('existing-a'));
var_dump($cache->fetch('existing-b'));
var_dump($cache->has('new-key'));

$fresh = new Opcache\UserCache('store-multiple-rollback-fresh');
var_dump($fresh->storeMultiple([
    'fresh-a' => 'new-a',
    'fresh-b' => str_repeat('x', 4 * 1024 * 1024),
]));
var_dump($fresh->has('fresh-a'));
var_dump($fresh->has('fresh-b'));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
string(5) "old-a"
string(5) "old-b"
bool(false)
bool(false)
bool(false)
bool(false)
