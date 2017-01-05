--TEST--
Bug #55719 (Argument restriction should come with a more specific error message)
--FILE--
<?php
Abstract Class Base {
	public function test($foo, array &$bar, $option = NULL, $extra = 3.141592653589793238462643383279502884197169399375105  ) {
	}	
}

class Sub extends Base {
	public function test($foo, array &$bar) {
	}	
}
?>
--EXPECTF--
Warning: Declaration of Sub::test($foo, array &$bar) should be compatible with Base::test($foo, array &$bar, $option = NULL, $extra = 3.1415926535898) in %sargument_restriction_002.php on line %d
