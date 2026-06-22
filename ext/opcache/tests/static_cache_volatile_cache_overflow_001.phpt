--TEST--
OPcache stable cache expunges entries under memory pressure without wiping oversized writes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=8
--FILE--
<?php

OPcache\StableCache::getInstance('default')->clear();

$payload = str_repeat('A', 1000000);
for ($i = 0; $i < 5; $i++) {
	var_dump(OPcache\StableCache::getInstance('default')->storeWithTtl('expired_' . $i, $payload, 1));
}

sleep(2);

var_dump(OPcache\StableCache::getInstance('default')->store('after_expired', str_repeat('B', 3000000)));
var_dump(strlen(OPcache\StableCache::getInstance('default')->fetch('after_expired')));

OPcache\StableCache::getInstance('default')->clear();
var_dump(OPcache\StableCache::getInstance('default')->store('small', 'survives'));
var_dump(OPcache\StableCache::getInstance('default')->store('huge', str_repeat('H', 12 * 1024 * 1024)));
var_dump(OPcache\StableCache::getInstance('default')->fetch('small'));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(3000000)
bool(true)
bool(false)
string(8) "survives"
