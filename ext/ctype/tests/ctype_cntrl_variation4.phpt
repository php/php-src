--TEST--
Test ctype_cntrl() function : usage variations - Octal and hexadecimal values
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_cntrl(mixed $c)
 * Description: Checks for control character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass hexadecimal and octal values to ctype_cntrl() to test behaviour
 */

echo "*** Testing ctype_cntrl() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

$octal_values = array(01, 02, 03, 04);
$hex_values = array(0x1, 0x2, 0x3, 0x4);

echo "\n-- Octal Values --\n";
$iterator = 1;
foreach($octal_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_cntrl($c));
	$iterator++;
}

echo "\n-- Hexadecimal Values --\n";
$iterator = 1;
foreach($hex_values as $c) {
	echo "-- Iteration $iterator --\n";
	var_dump(ctype_cntrl($c));
	$iterator++;
}
setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_cntrl() : usage variations ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_cntrl_variation4.php on line 13

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

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_cntrl_variation4.php on line 33
===DONE===
