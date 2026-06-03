--TEST--
OPcache stable cache TTL store APIs reject negative TTL before storing entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

OPcache\StableCache::getInstance('default')->clear();

try {
	OPcache\StableCache::getInstance('default')->storeWithTtl('single', 'stored', -1);
} catch (ValueError $exception) {
	echo $exception->getMessage(), "\n";
}

var_dump(OPcache\StableCache::getInstance('default')->fetch('single', 'missing'));

try {
	OPcache\StableCache::getInstance('default')->storeMultipleWithTtl(['array_first' => 'stored'], -1);
} catch (ValueError $exception) {
	echo $exception->getMessage(), "\n";
}

var_dump(OPcache\StableCache::getInstance('default')->fetch('array_first', 'missing'));

?>
--EXPECT--
OPcache\StableCache::storeWithTtl(): Argument #3 ($ttl) must be greater than or equal to 0
string(7) "missing"
OPcache\StableCache::storeMultipleWithTtl(): Argument #2 ($ttl) must be greater than or equal to 0
string(7) "missing"
