--TEST--
Bug #80564: ReflectionProperty::__toString() renders current value, not default value
--FILE--
<?php

class A {
	public static $x = 'default';
}

A::$x = new stdClass;

$rp = new ReflectionProperty('A', 'x');
echo $rp;

?>
--EXPECT--
Property [ public static $x = 'default' ]
