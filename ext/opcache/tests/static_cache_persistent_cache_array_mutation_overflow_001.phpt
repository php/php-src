--TEST--
OPcache PersistentStatic throws an exception when array mutation exhausts persistent cache shared memory
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.persistent_size_mb=8
opcache.optimization_level=0
opcache.file_update_protection=0
opcache.jit=0
--FILE--
<?php

class PersistentStaticCacheArrayMutationOverflow
{
	#[OPcache\PersistentStatic]
	public static array $value = [];
}

PersistentStaticCacheArrayMutationOverflow::$value = [];
try {
	PersistentStaticCacheArrayMutationOverflow::$value[] = str_repeat('X', 12 * 1024 * 1024);
} catch (OPcache\StaticCacheException $exception) {
	echo get_class($exception), ': ', $exception->getMessage(), "\n";
	OPcache\persistent_clear();
}

?>
--EXPECT--
OPcache\StaticCacheException: not enough shared memory left
