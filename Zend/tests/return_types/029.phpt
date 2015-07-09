--TEST--
Return types must not double free loop variables
--FILE--
<?php

function foo(): string {
	foreach ([new stdClass] as $class) {
		try {
			return $class; // invalid return type
		} catch (TypeError $e) {
			return "BAG!";
		}
	}
}
try {
	print foo();
} catch (TypeError $e) {
	print "no leak or segfault";
}
?>
--EXPECT--
no leak or segfault
