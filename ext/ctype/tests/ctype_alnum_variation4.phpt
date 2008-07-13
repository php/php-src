--TEST--
Test ctype_alnum() function : usage variations  - octal and hexadecimal values
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_alnum(mixed $c)
 * Description: Checks for alphanumeric character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass octal and hexadecimal values to ctype_alnum() to test behaviour
 */

echo "*** Testing ctype_alnum() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C"); 

$octal_values = array (060, 061, 062, 063);
$hex_values = array(0x30, 0x31, 0x32, 0x33);

echo "\n-- Octal Values --\n";
$iterator = 1;
foreach($octal_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_alnum($c));
	$iterator++;
}

echo "\n-- Hexadecimal Values --\n";
$iterator = 1;
foreach($hex_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_alnum($c));
	$iterator++;
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_alnum() : usage variations ***

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
