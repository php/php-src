--TEST--
Bug #26900 (Memory leak when passing arguments to __clone())
--FILE--
<?php
	class foo {
		function __clone() {}
	}
    
	$a = new foo;
	$b = $a->__clone('bad argument');

	echo "OK\n";
?>
--EXPECT--
OK
