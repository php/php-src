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
Fatal error: Default value for properties with callable type can only be null in %styped_properties_054.php on line 3
