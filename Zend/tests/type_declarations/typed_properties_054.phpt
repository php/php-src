--TEST--
Default values of callable properties
--FILE--
<?php
class A {
	public callable $a1 = 42;
}
$obj = new A;
var_dump($obj);
?>
--EXPECTF--
Fatal error: Property of type callable may not have default value in %s on line %d
