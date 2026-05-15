--TEST--
OPcache __DirectCacheSafe is visible but unavailable to userland classes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

var_dump(class_exists(OPcache\__DirectCacheSafe::class, false));
var_dump(in_array('OPcache\\__DirectCacheSafe', get_declared_classes(), true));

$marker = new ReflectionClass(OPcache\__DirectCacheSafe::class);
var_dump($marker->isInternal());
var_dump($marker->isFinal());

var_dump(count((new ReflectionClass(DateTimeImmutable::class))->getAttributes(OPcache\__DirectCacheSafe::class)));

$value = new DateTimeImmutable('2026-01-02 03:04:05', new DateTimeZone('UTC'));
var_dump(OPcache\volatile_store('hidden_safe_direct_datetime', $value));
var_dump(OPcache\volatile_fetch('hidden_safe_direct_datetime')->format('Y-m-d H:i:s e'));

require __DIR__ . '/static_cache_volatile_cache_direct_cache_safe_visibility_001.inc';

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
int(1)
bool(true)
string(23) "2026-01-02 03:04:05 UTC"

Fatal error: Only internal classes can be marked with #[OPcache\__DirectCacheSafe] in %sstatic_cache_volatile_cache_direct_cache_safe_visibility_001.inc on line %d
