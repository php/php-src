--TEST--
UserCache\Cache: lock re-entry within a request and unlock of unheld keys
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('lock-reentrancy');

var_dump($cache->lock('key'));
var_dump($cache->lock('key'));
var_dump($cache->unlock('key'));
var_dump($cache->unlock('key'));

var_dump($cache->unlock('never-locked'));

try {
    $cache->lock('key', -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
UserCache\Cache::lock(): Argument #2 ($lease) must be greater than or equal to 0
