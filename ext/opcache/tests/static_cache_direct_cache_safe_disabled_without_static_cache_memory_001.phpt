--TEST--
OPcache direct cache marker is not exposed when static cache memory is disabled
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=0
opcache.static_cache.pinned_size_mb=0
--FILE--
<?php

$marker = 'OPcache\\__DirectCacheSafe';
var_dump(class_exists($marker, false));
var_dump(count((new ReflectionClass(DateTime::class))->getAttributes($marker)));
var_dump(count((new ReflectionClass(DateTimeImmutable::class))->getAttributes($marker)));
var_dump(count((new ReflectionClass(SplFixedArray::class))->getAttributes($marker)));
var_dump(count((new ReflectionClass(ArrayObject::class))->getAttributes($marker)));

?>
--EXPECT--
bool(false)
int(0)
int(0)
int(0)
int(0)
