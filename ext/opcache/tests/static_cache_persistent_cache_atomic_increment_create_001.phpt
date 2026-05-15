--TEST--
OPcache persistent atomic increment creates missing keys without TTL
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.persistent_size_mb=32
--FILE--
<?php

use OPcache\StaticCacheException;

OPcache\persistent_clear();

var_dump(OPcache\persistent_atomic_increment('created', 7));
var_dump(OPcache\persistent_fetch('created'));
var_dump(OPcache\persistent_atomic_increment('created', 3));
var_dump(OPcache\persistent_fetch('created'));

var_dump(OPcache\persistent_lock('reserved'));
var_dump(OPcache\persistent_atomic_increment('reserved', 11));
var_dump(OPcache\persistent_fetch('reserved'));
var_dump(OPcache\persistent_lock('reserved'));

try {
	OPcache\persistent_atomic_decrement('missing_down');
} catch (StaticCacheException $exception) {
	echo $exception->getMessage(), "\n";
}

try {
	OPcache\persistent_atomic_increment('extra', 1, 1);
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
Cache key "missing_down" was not found
too-many-args
