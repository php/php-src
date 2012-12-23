--TEST--
Disallow multiple deprecated modifiers
--FILE--
<?php

class MyClass {
	deprecated public static deprecated function b() {
		echo "Called\n";	
	}
}

MyClass::b();

?>
--EXPECTF--	
Fatal error: Multiple deprecated modifiers are not allowed in %sdeprecated_functions_003.php on line 4