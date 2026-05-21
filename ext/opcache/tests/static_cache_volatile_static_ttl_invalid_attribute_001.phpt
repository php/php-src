--TEST--
OPcache VolatileStatic rejects negative TTL attributes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
--FILE--
<?php

#[OPcache\VolatileStatic(ttl: -1)]
class VolatileStaticNegativeTtl
{
}

?>
--EXPECTF--
Fatal error: OPcache\VolatileStatic ttl must be greater than or equal to 0 in %s on line %d
