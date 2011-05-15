--TEST--
Make sure trait does not implement an interface.
--FILE--
<?php
error_reporting(E_ALL);

interface MyInterface {
	public function a();
}

trait THello implements MyInterface {
  public function a() {
    echo 'A';
  }
}

?>
--EXPECTF--	
Fatal error: Cannot use 'MyInterface' as interface on 'THello' since it is a Trait in %s on line %d