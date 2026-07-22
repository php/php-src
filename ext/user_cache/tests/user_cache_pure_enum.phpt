--TEST--
UserCache\Cache: pure (non-backed) enum cases round-trip as singletons
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
$cache = UserCache\Cache::getPool('pure-enum');

enum PureSuit
{
    case Hearts;
    case Spades;
}

var_dump($cache->store('single', PureSuit::Hearts));
var_dump($cache->fetch('single') === PureSuit::Hearts);

var_dump($cache->store('list', [PureSuit::Hearts, PureSuit::Spades, PureSuit::Hearts]));
$fetched = $cache->fetch('list');
var_dump($fetched[0] === PureSuit::Hearts);
var_dump($fetched[1] === PureSuit::Spades);
var_dump($fetched[2] === PureSuit::Hearts);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
