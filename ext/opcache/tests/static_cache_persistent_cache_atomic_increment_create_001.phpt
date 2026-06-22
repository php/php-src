--TEST--
OPcache stable atomic increment and decrement create missing keys
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

OPcache\StableCache::getInstance('default')->clear();

var_dump(OPcache\StableCache::getInstance('default')->increment('created', 7));
var_dump(OPcache\StableCache::getInstance('default')->fetch('created'));
var_dump(OPcache\StableCache::getInstance('default')->increment('created', 3));
var_dump(OPcache\StableCache::getInstance('default')->fetch('created'));

var_dump(OPcache\StableCache::getInstance('default')->lock('reserved'));
var_dump(OPcache\StableCache::getInstance('default')->increment('reserved', 11));
var_dump(OPcache\StableCache::getInstance('default')->fetch('reserved'));
var_dump(OPcache\StableCache::getInstance('default')->lock('reserved'));
var_dump(OPcache\StableCache::getInstance('default')->decrement('missing_down'));
var_dump(OPcache\StableCache::getInstance('default')->fetch('missing_down'));
var_dump(OPcache\StableCache::getInstance('default')->decrement('missing_down', 4));
var_dump(OPcache\StableCache::getInstance('default')->fetch('missing_down'));

try {
	OPcache\StableCache::getInstance('default')->increment('extra', 1, false, false);
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
