--TEST--
The compatibility with the signature of abstract methods should be checked. (also checking the second possible implementation branch)
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
	use THelloA;
	use THelloB;
}



?>
--EXPECTF--	
Fatal error: Declaration of THelloA::hello($a) must be compatible with THelloB::hello() in %s on line %d