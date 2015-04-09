--TEST--
errmsg: static abstract function 
--FILE--
<?php

class test {
	static abstract function foo ();
}

echo "Done\n";
?>
--EXPECTF--	
Strict Standards: Static function test::foo() should not be abstract in %s on line %d

Fatal error: Class test contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (test::foo) in %s on line %d
