--TEST--
ReflectionClass::hasProperty() - error cases
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
	public $a;
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
var_dump($rc->hasProperty());
var_dump($rc->hasProperty("a", "a"));
var_dump($rc->hasProperty(null));
var_dump($rc->hasProperty(1));
var_dump($rc->hasProperty(1.5));
var_dump($rc->hasProperty(true));
var_dump($rc->hasProperty(array(1,2,3)));
var_dump($rc->hasProperty(new C));
?>
--EXPECTF--
Check invalid params:

Warning: ReflectionClass::hasProperty() expects exactly 1 parameter, 0 given in %s on line 8
NULL

Warning: ReflectionClass::hasProperty() expects exactly 1 parameter, 2 given in %s on line 9
NULL
bool(false)
bool(false)
bool(false)
bool(false)

Warning: ReflectionClass::hasProperty() expects parameter 1 to be string, array given in %s on line 14
NULL

Warning: ReflectionClass::hasProperty() expects parameter 1 to be string, object given in %s on line 15
NULL
