--TEST--
Bug #60174 (Notice when array in method prototype error)
--FILE--
<?php
Abstract Class Base {
	public function test($foo, $extra = array("test")) {
	}	
}

class Sub extends Base {
	public function test($foo, $extra) {
	}	
}
?>
--EXPECTF--
Warning: Declaration of Sub::test($foo, $extra) should be compatible with Base::test($foo, $extra = Array) in %sargument_restriction_006.php on line %d
