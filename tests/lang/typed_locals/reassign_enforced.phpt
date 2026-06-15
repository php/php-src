--TEST--
Typed local variables: type enforcement (with coercion) on reassignment
--FILE--
<?php
// int: reassignment with a numeric string coerces to int
int $x = 1;
$x = "7";
var_dump($x);

// string: reassignment with an int coerces to string
string $s = "a";
$s = 5;
var_dump($s);
?>
--EXPECT--
int(7)
string(1) "5"
