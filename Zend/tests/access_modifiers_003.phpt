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
Parse error: parse error, expecting `T_CLASS' in %s on line %d
