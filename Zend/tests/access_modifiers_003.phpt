--TEST--
using multiple access modifiers (classes)
--FILE--
<?php

final final class test {
	function foo() {}
}

echo "Done\n";
?>
--EXPECTF--	
Parse error: %s error,%sexpecting %s in %s on line %d
