--TEST--
OPcache pinned_store throws StaticCacheException when pinned cache memory is exhausted
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.pinned_size_mb=8
--FILE--
<?php

use OPcache\StaticCacheException;

OPcache\pinned_store('small', 'ok');

try {
	OPcache\pinned_store('huge', str_repeat('H', 12 * 1024 * 1024));
} catch (StaticCacheException $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump(OPcache\pinned_fetch('small', 'fallback'));
var_dump(OPcache\pinned_fetch('missing', 'fallback'));

?>
--EXPECT--
OPcache\StaticCacheException: not enough shared memory left
string(2) "ok"
string(8) "fallback"
