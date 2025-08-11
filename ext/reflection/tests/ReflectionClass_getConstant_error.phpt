--TEST--
ReflectionClass::getConstant() - bad params
--FILE--
<?php
class C {
    const myConst = 1;
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
var_dump($rc->getConstant(1));
var_dump($rc->getConstant(1.5));
var_dump($rc->getConstant(true));
?>
--EXPECTF--
Check invalid params:

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)
