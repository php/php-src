--TEST--
Abstract Trait Methods should behave like common abstract methods.
--FILE--
<?php
error_reporting(E_ALL);

trait THello {
  public abstract function hello();
}

trait THelloImpl {
  public function hello() {
    echo 'Hello';
  }
}

class TraitsTest {
	use THello;
	use THelloImpl;
}

$test = new TraitsTest();
$test->hello();
?>
--EXPECTF--	
Hello