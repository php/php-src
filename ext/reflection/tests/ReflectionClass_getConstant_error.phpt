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
bool(false)
bool(false)

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
bool(false)
