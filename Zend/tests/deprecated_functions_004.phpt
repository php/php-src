--TEST--
Disallow deprecated properties
--FILE--
<?php

class MyClass {
	deprecated public $a = 7;

	public function b() {
		echo "Called\n";
	}
}

MyClass::b();

?>
--EXPECTF--	
Fatal error: Properties cannot be declared deprecated in %sdeprecated_functions_004.php on line 4