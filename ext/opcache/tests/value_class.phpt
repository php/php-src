--TEST--
OPcache preserves the value class flag and semantics when linking cached classes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
$file = __DIR__ . '/value_class.inc';
var_dump(opcache_compile_file($file), opcache_is_script_cached($file));
require $file;
$reflection = new ReflectionClass(CachedBookingId::class);
var_dump($reflection->isValue(), $reflection->isFinal(), $reflection->isReadOnly());
$id = new CachedBookingId('booking-123');
var_dump($id->toString(), $id instanceof ValueIdentifier);
var_dump($id == clone $id, $id === clone $id);
var_dump((new ReflectionObject(unserialize(serialize($id))))->isValue());
try {
    $id->value = 'changed';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(11) "booking-123"
bool(true)
bool(true)
bool(false)
bool(true)
Cannot modify readonly property CachedBookingId::$value
