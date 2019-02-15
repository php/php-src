--TEST--
ReflectionClass::hasConstant() - error cases
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
	const myConst = 1;
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
var_dump($rc->hasConstant());
var_dump($rc->hasConstant("myConst", "myConst"));
var_dump($rc->hasConstant(null));
var_dump($rc->hasConstant(1));
var_dump($rc->hasConstant(1.5));
var_dump($rc->hasConstant(true));
var_dump($rc->hasConstant(array(1,2,3)));
var_dump($rc->hasConstant(new C));
?>
--EXPECTF--
Check invalid params:

Warning: ReflectionClass::hasConstant() expects exactly 1 parameter, 0 given in %s on line 8
NULL

Warning: ReflectionClass::hasConstant() expects exactly 1 parameter, 2 given in %s on line 9
NULL
bool(false)
bool(false)
bool(false)
bool(false)

Warning: ReflectionClass::hasConstant() expects parameter 1 to be string, array given in %s on line 14
NULL

Warning: ReflectionClass::hasConstant() expects parameter 1 to be string, object given in %s on line 15
NULL
