--TEST--
Test ctype_print() function : usage variations - octal and hexadecimal values
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_print(mixed $c)
 * Description: Checks for printable character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass octal and hexadecimal values to ctype_print() to test behaviour
 */

echo "*** Testing ctype_print() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

$octal_values = array(040, 041, 042, 043);
$hex_values = array (0x20, 0x21, 0x23, 0x24);

echo "\n-- Octal Values --\n";
$iterator = 1;
foreach($octal_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_print($c));
	$iterator++;
}

echo "\n-- Hexadecimal Values --\n";
$iterator = 1;
foreach($hex_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_print($c));
	$iterator++;
}
setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_print() : usage variations ***

-- Octal Values --
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)

-- Hexadecimal Values --
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)
===DONE===
