--TEST--
OPcache volatile cache: objects whose class is ZEND_ACC_NOT_SERIALIZABLE (Fiber, Generator, ...) are refused at store, matching serialize(), instead of being stored and read back broken
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
use OPcache\VolatileCache as VC;

$fiber = new Fiber(function () { Fiber::suspend(); });
$fiber->start();

function gen() { yield 1; }
$generator = gen();
$generator->current();

/* Non-serializable objects are refused (set returns false, nothing stored),
 * rather than stored and decoded into a broken instance. */
echo "fiber set: ";
var_dump(VC::getInstance('default')->store('k_fiber', $fiber));
echo "fiber stored: ";
var_dump(VC::getInstance('default')->fetch('k_fiber'));

echo "generator set: ";
var_dump(VC::getInstance('default')->store('k_gen', $generator));
echo "generator stored: ";
var_dump(VC::getInstance('default')->fetch('k_gen'));

/* Nested inside an array: the whole value is refused. */
echo "nested set: ";
var_dump(VC::getInstance('default')->store('k_nested', ['x' => $fiber, 'y' => 1]));
echo "nested stored: ";
var_dump(VC::getInstance('default')->fetch('k_nested'));

/* A storable value alongside still works. */
echo "control set: ";
var_dump(VC::getInstance('default')->store('k_ok', ['a' => 1, 'b' => [2, 3]]));
echo "control value: ", VC::getInstance('default')->fetch('k_ok')['b'][1], "\n";
?>
--EXPECT--
fiber set: bool(false)
fiber stored: NULL
generator set: bool(false)
generator stored: NULL
nested set: bool(false)
nested stored: NULL
control set: bool(true)
control value: 3
