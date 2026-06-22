--TEST--
OPcache StableStatic throws an exception when array mutation exhausts stable cache shared memory
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=8
opcache.optimization_level=0
opcache.file_update_protection=0
opcache.jit=0
--FILE--
<?php

class StableStaticCacheArrayMutationOverflow
{
	#[OPcache\StableStatic]
	public static array $value = [];
}

StableStaticCacheArrayMutationOverflow::$value = [];
try {
	StableStaticCacheArrayMutationOverflow::$value[] = str_repeat('X', 12 * 1024 * 1024);
} catch (OPcache\StaticCacheException $exception) {
	echo get_class($exception), ': ', $exception->getMessage(), "\n";
	OPcache\StableCache::getInstance('default')->clear();
}

?>
--EXPECT--
OPcache\StaticCacheException: not enough shared memory left
