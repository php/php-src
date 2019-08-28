--TEST--
Memory leak when returning TMP/VAR with wrong return type
--FILE--
<?php

function foo(): StdClass {
	$a = new StdClass;
	$b = [];
	return [$a, $b];
}

try {
	foo();
} catch (Error $e) {
	echo $e->getMessage(), " in ", $e->getFile(), " on line ", $e->getLine();
}

?>
--EXPECTF--
Return value of foo() must be an instance of StdClass, array returned in %s on line %d
