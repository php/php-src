--TEST--
Abstract Trait Methods should behave like common abstract methods.
--FILE--
<?php
error_reporting(E_ALL);

trait THello {
  public abstract function hello();
}

class TraitsTest {
	use THello;  
}

$test = new TraitsTest();
$test->hello();
?>
--EXPECTF--	
Fatal error: Class %s contains %d abstract method and must therefore be declared abstract or implement the remaining methods (%s) in %s on line %d