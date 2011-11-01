--TEST--
The compatibility with the signature of abstract methods should be checked.
--FILE--
<?php
error_reporting(E_ALL);

trait THelloB {
  public function hello() {
	echo 'Hello';
  }
}

trait THelloA {
  public abstract function hello($a);
}

class TraitsTest1 {
	use THelloB;
	use THelloA;  
}


?>
--EXPECTF--	
Fatal error: Declaration of THelloB::hello() must be compatible with THelloA::hello($a) in %s on line %d