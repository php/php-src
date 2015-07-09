--TEST--
Return types must not double free loop variables
--FILE--
<?php

function foo(): string {
	foreach ([new stdClass] as $class) {
		try {
			return $class; // invalid return type
		} catch (TypeError $e) {
			return "no leak or segfault";
		}
	}
}
print foo();

?>
--EXPECT--
no leak or segfault
