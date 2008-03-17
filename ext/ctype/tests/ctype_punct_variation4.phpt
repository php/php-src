--TEST--
Test ctype_punct() function : usage variations - Octal and Hexadecimal values
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_punct(mixed $c)
 * Description: Checks for any printable character which is not whitespace 
 * or an alphanumeric character 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different octal and hexadecimal values to ctype_punct() to test behaviour
 */

echo "*** Testing ctype_punct() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C"); 

$octal_values = array(045, 046, 047, 050);
$hex_values   = array(0x25, 0x26, 0x27, 0x28);

echo "\n-- Octal Values --\n";
$iterator = 1;
foreach($octal_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_punct($c));
	$iterator++;
}

echo "\n-- Hexadecimal Values --\n";
$iterator = 1;
foreach($hex_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_punct($c));
	$iterator++;
}

setlocale(LC_CTYPE, $orig); 
?>
===DONE===
--EXPECTF--
*** Testing ctype_punct() : usage variations ***

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
