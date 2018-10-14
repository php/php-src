--TEST--
ReflectionClass::hasConstant()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class C {
	const myConst = 1;
}

class D extends C {
}


$rc = new ReflectionClass("C");
echo "Check existing constant: ";
var_dump($rc->hasConstant("myConst"));
echo "Check existing constant, different case: ";
var_dump($rc->hasConstant("MyCoNsT"));
echo "Check absent constant: ";
var_dump($rc->hasConstant("doesntExist"));


$rd = new ReflectionClass("D");
echo "Check inherited constant: ";
var_dump($rd->hasConstant("myConst"));
echo "Check absent constant: ";
var_dump($rd->hasConstant("doesntExist"));
?>
--EXPECTF--
Check existing constant: bool(true)
Check existing constant, different case: bool(false)
Check absent constant: bool(false)
Check inherited constant: bool(true)
Check absent constant: bool(false)
