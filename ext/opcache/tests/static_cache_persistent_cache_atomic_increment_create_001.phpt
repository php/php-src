--TEST--
OPcache persistent atomic increment and decrement create missing keys
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.persistent_size_mb=32
--FILE--
<?php

OPcache\persistent_clear();

var_dump(OPcache\persistent_atomic_increment('created', 7));
var_dump(OPcache\persistent_fetch('created'));
var_dump(OPcache\persistent_atomic_increment('created', 3));
var_dump(OPcache\persistent_fetch('created'));

var_dump(OPcache\persistent_lock('reserved'));
var_dump(OPcache\persistent_atomic_increment('reserved', 11));
var_dump(OPcache\persistent_fetch('reserved'));
var_dump(OPcache\persistent_lock('reserved'));
var_dump(OPcache\persistent_atomic_decrement('missing_down'));
var_dump(OPcache\persistent_fetch('missing_down'));
var_dump(OPcache\persistent_atomic_decrement('missing_down', 4));
var_dump(OPcache\persistent_fetch('missing_down'));

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
int(-1)
int(-1)
int(-5)
int(-5)
too-many-args
