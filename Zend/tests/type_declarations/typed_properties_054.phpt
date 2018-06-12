--TEST--
Typed properties disallow callable (nullable variant)
--FILE--
<?php
class A {
	public ?callable $a;
}
$obj = new A;
var_dump($obj);
?>
--EXPECTF--
Fatal error: Typed property A::$a cannot have type callable in %s on line %d
