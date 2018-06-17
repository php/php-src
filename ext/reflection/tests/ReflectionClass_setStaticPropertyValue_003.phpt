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
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump(Test::$y);

$rc->setStaticPropertyValue("y", "21");
var_dump(Test::$y);


Test::$x =& Test::$y;

try {
	$rc->setStaticPropertyValue("x", "foo");
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump(Test::$y);

$rc->setStaticPropertyValue("x", "42");
var_dump(Test::$y);

?>
--EXPECT--
string(48) "Typed property Test::$y must be int, string used"
int(2)
int(21)
string(45) "Cannot assign string to reference of type int"
int(21)
int(42)
