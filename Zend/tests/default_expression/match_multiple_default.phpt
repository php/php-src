--TEST--
Tests declaring multiple default match arms is still prohibited in an argument context
--FILE--
<?php

function F($V = 1) {}
F(
	match (0) {
		default => 1,
		default => 2,
	}
);
?>
--EXPECTF--
Fatal error: Match expressions may only contain one default arm in %s on line %d
