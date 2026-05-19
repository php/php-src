--TEST--
OPcache pinned atomic operations reject non-integer entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

use OPcache\StaticCacheException;

OPcache\pinned_store('text', 'php');

try {
	OPcache\pinned_atomic_increment('text');
} catch (StaticCacheException $exception) {
	echo $exception->getMessage(), "\n";
}

try {
	OPcache\pinned_atomic_decrement('text');
} catch (StaticCacheException $exception) {
	echo $exception->getMessage(), "\n";
}

?>
--EXPECT--
Atomic increment requires an integer value
Atomic decrement requires an integer value
