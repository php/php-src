--TEST--
OPcache persistent atomic operations reject non-integer entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.persistent_size_mb=32
--FILE--
<?php

use OPcache\StaticCacheException;

OPcache\persistent_store('text', 'php');

try {
	OPcache\persistent_atomic_increment('text');
} catch (StaticCacheException $exception) {
	echo $exception->getMessage(), "\n";
}

try {
	OPcache\persistent_atomic_decrement('text');
} catch (StaticCacheException $exception) {
	echo $exception->getMessage(), "\n";
}

?>
--EXPECT--
Atomic increment requires an integer value
Atomic decrement requires an integer value
