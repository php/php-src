--TEST--
Visibility modifiers: Using multiple access type modifiers (interfaces)
--FILE--
<?php

public private interface test {
	function foo();
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
