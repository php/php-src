--TEST--
OPcache __DirectCacheSafe does not touch internal classes when static cache memory is disabled
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

var_dump(count((new ReflectionClass(DateTime::class))->getAttributes(OPcache\__DirectCacheSafe::class)));
var_dump(count((new ReflectionClass(DateTimeImmutable::class))->getAttributes(OPcache\__DirectCacheSafe::class)));
var_dump(count((new ReflectionClass(SplFixedArray::class))->getAttributes(OPcache\__DirectCacheSafe::class)));
var_dump(count((new ReflectionClass(ArrayObject::class))->getAttributes(OPcache\__DirectCacheSafe::class)));

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
