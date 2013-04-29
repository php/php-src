--TEST--
Bug #55719 (Argument restriction should come with a more specific error message)
--FILE--
<?php
Class Base {
	public function &test($foo, array $bar, $option = NULL, $extra = "lllllllllllllllllllllllllllllllllllllllllllllllllll") {
	}	
}

class Sub extends Base {
	public function &test() {
	}	
}
?>
--EXPECTF--
Strict Standards: Declaration of Sub::test() should be compatible with & Base::test($foo, array $bar, $option = NULL, $extra = 'llllllllll...') in %sargument_restriction_001.php on line %d 
