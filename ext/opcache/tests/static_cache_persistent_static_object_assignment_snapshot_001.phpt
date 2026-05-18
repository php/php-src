--TEST--
OPcache PersistentStatic snapshots object assignments without following object property writes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.persistent_size_mb=32
opcache.optimization_level=0
opcache.file_update_protection=0
opcache.jit=0
--FILE--
<?php

class NestedObjectPropertyState
{
	#[OPcache\PersistentStatic]
	public static ?stdClass $propertyState = null;
}

NestedObjectPropertyState::$propertyState ??= (object) ['count' => 1];
var_dump(NestedObjectPropertyState::$propertyState->count);
var_dump(OPcache\persistent_cache_info()->entry_count);

NestedObjectPropertyState::$propertyState->count++;
var_dump(NestedObjectPropertyState::$propertyState->count);
var_dump(OPcache\persistent_cache_info()->entry_count);

NestedObjectPropertyState::$propertyState->count = 10;
var_dump(NestedObjectPropertyState::$propertyState->count);
var_dump(OPcache\persistent_cache_info()->entry_count);

?>
--EXPECT--
int(1)
int(1)
int(2)
int(1)
int(10)
int(1)
