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
Warning: Array to string conversion in %s on line %d
runtime

Warning: Undefined property: A::$Array in %s on line %d
NULL

Warning: Undefined property: A::$1 in %s on line %d
NULL

Fatal error: Uncaught Error: Object of class Closure could not be converted to string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
