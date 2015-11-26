--TEST--
errmsg: multiple access type modifiers are not allowed (methods) 
--FILE--
<?php

class test {
	private protected function foo() {}
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
