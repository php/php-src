--TEST--
Visibility modifiers: Using multiple access type modifiers (classes)
--FILE--
<?php

public private class test {
	function foo() {}
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
