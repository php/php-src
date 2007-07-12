--TEST--
ReflectionObject::getConstant() - invalid params
--FILE--
<?php
class C {
	const myConst = 1;
}

$rc = new ReflectionObject(new C);
var_dump($rc->getConstant());
var_dump($rc->getConstant("myConst", "myConst"));
var_dump($rc->getConstant(null));
var_dump($rc->getConstant(1));
var_dump($rc->getConstant(1.5));
var_dump($rc->getConstant(true));
var_dump($rc->getConstant(array(1,2,3)));
var_dump($rc->getConstant(new C));
?>
--EXPECTF--
Warning: ReflectionClass::getConstant() expects exactly 1 parameter, 0 given in %s on line 7
NULL

Warning: ReflectionClass::getConstant() expects exactly 1 parameter, 2 given in %s on line 8
NULL
bool(false)
bool(false)
bool(false)
bool(false)

Warning: ReflectionClass::getConstant() expects parameter 1 to be string, array given in %s on line 13
NULL

Warning: ReflectionClass::getConstant() expects parameter 1 to be string, object given in %s on line 14
NULL
