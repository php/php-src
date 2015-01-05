--TEST--
Const class properties(runtime cache)
--FILE--
<?php
class A {
}

$a = new A;

echo "runtime\n";
var_dump($a->{array()});
var_dump($a->{1});
var_dump($a->{function(){}});
?>
--EXPECTF--
Notice: Array to string conversion in %sclass_properties_const.php on line %d
runtime

Notice: Undefined property: A::$Array in %sclass_properties_const.php on line %d
NULL

Notice: Undefined property: A::$1 in %sclass_properties_const.php on line %d
NULL

Catchable fatal error: Object of class Closure could not be converted to string in %sclass_properties_const.php on line %d
