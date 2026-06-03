--TEST--
OPcache stable cache TTL expiry and reuse
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

var_dump(OPcache\StableCache::getInstance('default')->storeWithTtl('ttl', 'one', 1));
var_dump(OPcache\StableCache::getInstance('default')->fetch('ttl'));

sleep(2);

var_dump(OPcache\StableCache::getInstance('default')->fetch('ttl', 'expired'));

var_dump(OPcache\StableCache::getInstance('default')->store('ttl', 'two'));
var_dump(OPcache\StableCache::getInstance('default')->fetch('ttl'));

?>
--EXPECT--
bool(true)
string(3) "one"
string(7) "expired"
bool(true)
string(3) "two"
