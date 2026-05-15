--TEST--
OPcache persistent_store throws StaticCacheException when persistent cache memory is exhausted
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.persistent_size_mb=8
--FILE--
<?php

use OPcache\StaticCacheException;

OPcache\persistent_store('small', 'ok');

try {
	OPcache\persistent_store('huge', str_repeat('H', 12 * 1024 * 1024));
} catch (StaticCacheException $e) {
	echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump(OPcache\persistent_fetch('small', 'fallback'));
var_dump(OPcache\persistent_fetch('missing', 'fallback'));

?>
--EXPECT--
OPcache\StaticCacheException: not enough shared memory left
string(2) "ok"
string(8) "fallback"
