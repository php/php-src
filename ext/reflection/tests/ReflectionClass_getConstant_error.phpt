--TEST--
ReflectionClass::getConstant() - bad params
--FILE--
<?php
class C {
    const myConst = 1;
}

$rc = new ReflectionClass("C");
echo "Check invalid params:\n";
var_dump($rc->getConstant(null));
var_dump($rc->getConstant(1));
var_dump($rc->getConstant(1.5));
var_dump($rc->getConstant(true));
?>
--EXPECT--
Check invalid params:
bool(false)
bool(false)
bool(false)
bool(false)
