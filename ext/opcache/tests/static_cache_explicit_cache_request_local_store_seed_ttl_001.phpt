--TEST--
OPcache explicit request-local store seed respects stable TTL expiry
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.stable_size_mb=32
--FILE--
<?php

opcache_static_cache_volatile_reset();
$stable = OPcache\StableCache::getInstance('store_seed_ttl');
$stable->clear();

$large = str_repeat('s', 300);
var_dump($stable->storeWithTtl('large', $large, 1));
echo strlen($stable->fetch('large', 'expired')), "\n";
sleep(2);
var_dump($stable->fetch('large', 'expired'));

$replacement = str_repeat('t', 300);
var_dump($stable->store('large', $replacement));
$fetched = $stable->fetch('large', 'expired');
echo strlen($fetched), ':', $fetched[0], "\n";

?>
--EXPECT--
bool(true)
300
string(7) "expired"
bool(true)
300:t
