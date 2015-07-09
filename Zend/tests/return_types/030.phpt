--TEST--
Return types must not double free loop variables
--FILE--
<?php

function foo(): string {
	$a = [];
	try {
		return $a; // invalid return type
	} catch (TypeError $e) {
		echo "BAG!\n";
		return "ops!\n";
	}
}
try {
	echo foo();
} catch (TypeError $e) {
	echo "OK\n";
}

?>
--EXPECT--
OK
