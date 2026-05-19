--TEST--
OPcache direct cache handlers are not exposed as a userland attribute
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

$marker = 'OPcache\\__DirectCacheSafe';
var_dump(class_exists($marker, false));
var_dump(in_array($marker, get_declared_classes(), true));
var_dump(count((new ReflectionClass(DateTimeImmutable::class))->getAttributes($marker)));

$value = new DateTimeImmutable('2026-01-02 03:04:05', new DateTimeZone('UTC'));
var_dump(OPcache\volatile_store('hidden_safe_direct_datetime', $value));
var_dump(OPcache\volatile_fetch('hidden_safe_direct_datetime')->format('Y-m-d H:i:s e'));

?>
--EXPECT--
bool(false)
bool(false)
int(0)
bool(true)
string(23) "2026-01-02 03:04:05 UTC"
