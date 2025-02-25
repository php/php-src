--TEST--
Tests declaring a default match arm adjacent to another condition is still prohibited in an argument context
--FILE--
<?php

function F($V = 1) {}
F(
	match (0) {
	    0,
		default => 1,
	}
);
?>
--EXPECTF--
Fatal error: Match arms may not share conditions with the default arm in %s on line %d
