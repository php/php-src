--TEST--
Typed properties disallow callable
--FILE--
<?php
class A {
    public callable $a;
}
$obj = new A;
var_dump($obj);
?>
--EXPECTF--
Fatal error: Property A::$a cannot have type callable in %s on line %d
