--TEST--
OPcache volatile cache expunges entries under memory pressure without wiping oversized writes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=8
--FILE--
<?php

OPcache\VolatileCache::clear();

$payload = str_repeat('A', 1000000);
for ($i = 0; $i < 5; $i++) {
	var_dump(OPcache\VolatileCache::set('expired_' . $i, $payload, 1));
}

sleep(2);

var_dump(OPcache\VolatileCache::set('after_expired', str_repeat('B', 3000000)));
var_dump(strlen(OPcache\VolatileCache::get('after_expired')));

OPcache\VolatileCache::clear();
var_dump(OPcache\VolatileCache::set('small', 'survives'));
var_dump(OPcache\VolatileCache::set('huge', str_repeat('H', 12 * 1024 * 1024)));
var_dump(OPcache\VolatileCache::get('small'));

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
