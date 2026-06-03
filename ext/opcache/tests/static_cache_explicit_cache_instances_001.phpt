--TEST--
OPcache explicit cache instances are final non-serializable multitons
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

function dump_error(string $label, callable $callback): void
{
	try {
		$callback();
		echo $label, ": no error\n";
	} catch (Throwable $e) {
		echo $label, ": ", get_class($e), ": ", $e->getMessage(), "\n";
	}
}

foreach ([OPcache\VolatileCache::class, OPcache\StableCache::class] as $class) {
	echo $class, "\n";
	$first = $class::getInstance('pool');
	$second = $class::getInstance('pool');
	$other = $class::getInstance('other');

	var_dump($first instanceof OPcache\StaticCacheInterface);
	var_dump($first === $second);
	var_dump($first === $other);

	dump_error('new', static fn () => new $class());
	dump_error('clone', static fn () => clone $first);
	dump_error('serialize', static fn () => serialize($first));
	dump_error('pool-delimiter', static fn () => $class::getInstance('bad:pool'));
}

?>
--EXPECT--
OPcache\VolatileCache
bool(true)
bool(true)
bool(false)
new: Error: Call to private OPcache\VolatileCache::__construct() from global scope
clone: Error: Trying to clone an uncloneable object of class OPcache\VolatileCache
serialize: Exception: Serialization of 'OPcache\VolatileCache' is not allowed
pool-delimiter: ValueError: OPcache\VolatileCache::getInstance(): Argument #1 ($pool_name) must not contain the static-cache key delimiter ":"
OPcache\StableCache
bool(true)
bool(true)
bool(false)
new: Error: Call to private OPcache\StableCache::__construct() from global scope
clone: Error: Trying to clone an uncloneable object of class OPcache\StableCache
serialize: Exception: Serialization of 'OPcache\StableCache' is not allowed
pool-delimiter: ValueError: OPcache\StableCache::getInstance(): Argument #1 ($pool_name) must not contain the static-cache key delimiter ":"
