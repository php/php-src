--TEST--
Test ctype_upper() function : usage variations - different integers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_upper(mixed $c)
 * Description: Checks for uppercase character(s)
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_upper() to test which character codes are considered
 * valid uppercase characters
 */

echo "*** Testing ctype_upper() : usage variations ***\n";
$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
	if (ctype_upper($i)) {
		echo "character code $i is a uppercase character\n";
	}
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECT--
*** Testing ctype_upper() : usage variations ***
character code 65 is a uppercase character
character code 66 is a uppercase character
character code 67 is a uppercase character
character code 68 is a uppercase character
character code 69 is a uppercase character
character code 70 is a uppercase character
character code 71 is a uppercase character
character code 72 is a uppercase character
character code 73 is a uppercase character
character code 74 is a uppercase character
character code 75 is a uppercase character
character code 76 is a uppercase character
character code 77 is a uppercase character
character code 78 is a uppercase character
character code 79 is a uppercase character
character code 80 is a uppercase character
character code 81 is a uppercase character
character code 82 is a uppercase character
character code 83 is a uppercase character
character code 84 is a uppercase character
character code 85 is a uppercase character
character code 86 is a uppercase character
character code 87 is a uppercase character
character code 88 is a uppercase character
character code 89 is a uppercase character
character code 90 is a uppercase character
===DONE===
