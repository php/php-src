--TEST--
UserCache\Cache: store retries can clear on memory pressure
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=1M
--FILE--
<?php
$cache = UserCache\Cache::getPool('pressure-clear');
$cache->clear();

var_dump($cache->store('marker', 'kept-until-pressure'));

for ($i = 0; $i < 220; $i++) {
    $cache->store('fill-' . $i, str_repeat(chr(65 + ($i % 26)), 4096));
}

$large = str_repeat('L', 256 * 1024);
var_dump($cache->store('large', $large));
var_dump($cache->fetch('large', '') === $large);
var_dump($cache->fetch('marker', 'missing'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(7) "missing"
