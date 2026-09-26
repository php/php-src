--TEST--
DateInterval::__wakeup() must not pass raw property-table zvals to zval_get_double()/zval_get_long()
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

/* date_object_get_properties_interval() returns the object's raw property table
 * when the interval is not initialized. Declared properties are IS_INDIRECT in
 * that table, which the conversion helpers do not accept. */

class DeclaredInitialized extends DateInterval { public float $f = 1.5; }
class DeclaredUninitialized extends DateInterval { public float $f; }
class DeclaredCivilOrWall extends DateInterval { public int $civil_or_wall = 1; }
#[AllowDynamicProperties] class Dynamic extends DateInterval {}

foreach (['DeclaredInitialized', 'DeclaredUninitialized', 'DeclaredCivilOrWall'] as $cls) {
    $o = (new ReflectionClass($cls))->newInstanceWithoutConstructor();
    $o->__wakeup();
    echo $cls, ": ok\n";
}

/* A dynamic property is a plain zval, so it is still read normally. */
$o = (new ReflectionClass('Dynamic'))->newInstanceWithoutConstructor();
$o->f = 0.25;
$o->__wakeup();
echo "Dynamic: ", $o->f, "\n";

/* The normal round-trip is unaffected. */
$i = new DateInterval('PT1S');
$i->f = 0.5;
$restored = unserialize(serialize($i));
echo "roundtrip f: ", $restored->f, "\n";
echo "roundtrip s: ", $restored->s, "\n";
?>
--EXPECT--
DeclaredInitialized: ok
DeclaredUninitialized: ok
DeclaredCivilOrWall: ok
Dynamic: 0.25
roundtrip f: 0.5
roundtrip s: 1
