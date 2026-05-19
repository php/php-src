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

OPcache\pinned_clear();

var_dump(OPcache\pinned_atomic_increment('created', 7));
var_dump(OPcache\pinned_fetch('created'));
var_dump(OPcache\pinned_atomic_increment('created', 3));
var_dump(OPcache\pinned_fetch('created'));

var_dump(OPcache\pinned_lock('reserved'));
var_dump(OPcache\pinned_atomic_increment('reserved', 11));
var_dump(OPcache\pinned_fetch('reserved'));
var_dump(OPcache\pinned_lock('reserved'));
var_dump(OPcache\pinned_atomic_decrement('missing_down'));
var_dump(OPcache\pinned_fetch('missing_down'));
var_dump(OPcache\pinned_atomic_decrement('missing_down', 4));
var_dump(OPcache\pinned_fetch('missing_down'));

try {
	OPcache\pinned_atomic_increment('extra', 1, false, false);
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
