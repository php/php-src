--TEST--
ReflectionClass::setStaticPropertyValue() - type constraints must be enforced
--FILE--
<?php

class Test {
    public static $x;
    public static int $y = 2;
}

$rc = new ReflectionClass('Test');

try {
    $rc->setStaticPropertyValue("y", "foo");
} catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump(Test::$y);

$rc->setStaticPropertyValue("y", "21");
var_dump(Test::$y);


Test::$x =& Test::$y;

try {
    $rc->setStaticPropertyValue("x", "foo");
} catch (Throwable $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump(Test::$y);

$rc->setStaticPropertyValue("x", "42");
var_dump(Test::$y);

?>
--EXPECT--
TypeError: Cannot assign string to property Test::$y of type int
int(2)
int(21)
TypeError: Cannot assign string to reference held by property Test::$y of type int
int(21)
int(42)
