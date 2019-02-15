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
Fatal error: Multiple final modifiers are not allowed in %s on line %d
