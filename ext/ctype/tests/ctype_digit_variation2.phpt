--TEST--
Test ctype_digit() function : usage variations - different integers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_digit(mixed $c)
 * Description: Checks for numeric character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_digit() to test which character codes are considered
 * valid decimal digits
 */

echo "*** Testing ctype_digit() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
	if (ctype_digit($i)) {
		echo "character code $i is a numeric digit\n";
	}
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_digit() : usage variations ***
character code 48 is a numeric digit
character code 49 is a numeric digit
character code 50 is a numeric digit
character code 51 is a numeric digit
character code 52 is a numeric digit
character code 53 is a numeric digit
character code 54 is a numeric digit
character code 55 is a numeric digit
character code 56 is a numeric digit
character code 57 is a numeric digit
===DONE===
