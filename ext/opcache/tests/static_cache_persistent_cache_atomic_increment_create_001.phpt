--TEST--
OPcache pinned atomic increment and decrement create missing keys
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

OPcache\PinnedCache::clear();

var_dump(OPcache\PinnedCache::increment('created', 7));
var_dump(OPcache\PinnedCache::get('created'));
var_dump(OPcache\PinnedCache::increment('created', 3));
var_dump(OPcache\PinnedCache::get('created'));

var_dump(OPcache\PinnedCache::lock('reserved'));
var_dump(OPcache\PinnedCache::increment('reserved', 11));
var_dump(OPcache\PinnedCache::get('reserved'));
var_dump(OPcache\PinnedCache::lock('reserved'));
var_dump(OPcache\PinnedCache::decrement('missing_down'));
var_dump(OPcache\PinnedCache::get('missing_down'));
var_dump(OPcache\PinnedCache::decrement('missing_down', 4));
var_dump(OPcache\PinnedCache::get('missing_down'));

try {
	OPcache\PinnedCache::increment('extra', 1, false, false);
} catch (ArgumentCountError $exception) {
	echo "too-many-args\n";
}

?>
--EXPECT--
int(7)
int(7)
int(10)
int(10)
bool(true)
int(11)
int(11)
bool(true)
int(-1)
int(-1)
int(-5)
int(-5)
too-many-args
