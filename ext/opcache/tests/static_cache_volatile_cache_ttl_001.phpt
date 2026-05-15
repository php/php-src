--TEST--
OPcache volatile cache TTL expiry and reuse
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

var_dump(OPcache\volatile_store('ttl', 'one', 1));
var_dump(OPcache\volatile_fetch('ttl'));

sleep(2);

var_dump(OPcache\volatile_fetch('ttl', 'expired'));

var_dump(OPcache\volatile_store('ttl', 'two'));
var_dump(OPcache\volatile_fetch('ttl'));

?>
--EXPECT--
bool(true)
string(3) "one"
string(7) "expired"
bool(true)
string(3) "two"
