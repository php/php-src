--TEST--
Tests default as an expression in a match condition is not treated as a default match arm
--FILE--
<?php

$F = fn ($V = 1) => $V;
var_dump($F(
	match (default) {
		default + 1 => 2,
		default => default,
	}
));
?>
--EXPECT--
int(1)
