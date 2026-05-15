--TEST--
OPcache volatile cache store APIs reject negative TTL before storing entries
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

OPcache\volatile_clear();

try {
	OPcache\volatile_store('single', 'stored', -1);
} catch (ValueError $exception) {
	echo $exception->getMessage(), "\n";
}

var_dump(OPcache\volatile_fetch('single', 'missing'));

try {
	OPcache\volatile_store_array(['array_first' => 'stored'], -1);
} catch (ValueError $exception) {
	echo $exception->getMessage(), "\n";
}

var_dump(OPcache\volatile_fetch('array_first', 'missing'));

?>
--EXPECT--
OPcache\volatile_store(): Argument #3 ($ttl) must be greater than or equal to 0
string(7) "missing"
OPcache\volatile_store_array(): Argument #2 ($ttl) must be greater than or equal to 0
string(7) "missing"
