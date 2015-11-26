--TEST--
ReflectionClass::hasMethod() - error cases
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
	function f() {}
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
var_dump($rc->hasMethod());
var_dump($rc->hasMethod("f", "f"));
var_dump($rc->hasMethod(null));
var_dump($rc->hasMethod(1));
var_dump($rc->hasMethod(1.5));
var_dump($rc->hasMethod(true));
var_dump($rc->hasMethod(array(1,2,3)));
var_dump($rc->hasMethod(new C));
?>
--EXPECTF--
Check invalid params:

Warning: ReflectionClass::hasMethod() expects exactly 1 parameter, 0 given in %s on line 8
NULL

Warning: ReflectionClass::hasMethod() expects exactly 1 parameter, 2 given in %s on line 9
NULL
bool(false)
bool(false)
bool(false)
bool(false)

Warning: ReflectionClass::hasMethod() expects parameter 1 to be string, array given in %s on line 14
NULL

Warning: ReflectionClass::hasMethod() expects parameter 1 to be string, object given in %s on line 15
NULL
