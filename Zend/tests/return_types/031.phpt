--TEST--
Excepton on return types mismutch shouldn't execute "catch" code but should execute "finally" code.
--FILE--
<?php

function foo(): string {
	$a = "OK\n";
	try {
		return $a; // invalid return type
	} catch (TypeError $e) {
		echo "BAG!\n";
		return "ops!\n";
	} finally {
		echo "finally1\n";
	}
}
function bar(): string {
	$a = [];
	try {
		return $a; // invalid return type
	} catch (TypeError $e) {
		echo "BAG!\n";
		return "ops!\n";
	} finally {
		echo "finally2\n";
	}
}
try {
	echo foo();
} catch (TypeError $e) {
	echo "BAG\n";
}
try {
	echo bar();
} catch (TypeError $e) {
	echo "OK\n";
}
?>
--EXPECT--
finally1
OK
finally2
OK
