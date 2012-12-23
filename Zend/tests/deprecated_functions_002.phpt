--TEST--
Declare deprecated method
--FILE--
<?php

class MyClass {
	deprecated public static function b() {
		echo "Called\n";	
	}
}

MyClass::b();

?>
--EXPECTF--
Deprecated: Function MyClass::b() is deprecated in %sdeprecated_functions_002.php on line 9
Called