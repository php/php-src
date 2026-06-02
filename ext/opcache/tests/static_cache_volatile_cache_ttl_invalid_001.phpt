--TEST--
OPcache volatile cache store APIs reject negative TTL before storing entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

OPcache\VolatileCache::clear();

try {
	OPcache\VolatileCache::set('single', 'stored', -1);
} catch (ValueError $exception) {
	echo $exception->getMessage(), "\n";
}

var_dump(OPcache\VolatileCache::get('single', 'missing'));

try {
	OPcache\VolatileCache::setMultiple(['array_first' => 'stored'], -1);
} catch (ValueError $exception) {
	echo $exception->getMessage(), "\n";
}

var_dump(OPcache\VolatileCache::get('array_first', 'missing'));

?>
--EXPECT--
OPcache\VolatileCache::set(): Argument #3 ($ttl) must be greater than or equal to 0
string(7) "missing"
OPcache\VolatileCache::setMultiple(): Argument #2 ($ttl) must be greater than or equal to 0
string(7) "missing"
