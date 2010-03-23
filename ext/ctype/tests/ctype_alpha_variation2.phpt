--TEST--
Test ctype_alpha() function : usage variations - different integers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_alpha(mixed $c)
 * Description: Checks for alphabetic character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_alpha() to test which character codes are considered
 * valid alphabetic characters
 */

echo "*** Testing ctype_alpha() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
	if (ctype_alpha($i)) {
		echo "character code $i is alphabetic\n";
	}
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_alpha() : usage variations ***
character code 65 is alphabetic
character code 66 is alphabetic
character code 67 is alphabetic
character code 68 is alphabetic
character code 69 is alphabetic
character code 70 is alphabetic
character code 71 is alphabetic
character code 72 is alphabetic
character code 73 is alphabetic
character code 74 is alphabetic
character code 75 is alphabetic
character code 76 is alphabetic
character code 77 is alphabetic
character code 78 is alphabetic
character code 79 is alphabetic
character code 80 is alphabetic
character code 81 is alphabetic
character code 82 is alphabetic
character code 83 is alphabetic
character code 84 is alphabetic
character code 85 is alphabetic
character code 86 is alphabetic
character code 87 is alphabetic
character code 88 is alphabetic
character code 89 is alphabetic
character code 90 is alphabetic
character code 97 is alphabetic
character code 98 is alphabetic
character code 99 is alphabetic
character code 100 is alphabetic
character code 101 is alphabetic
character code 102 is alphabetic
character code 103 is alphabetic
character code 104 is alphabetic
character code 105 is alphabetic
character code 106 is alphabetic
character code 107 is alphabetic
character code 108 is alphabetic
character code 109 is alphabetic
character code 110 is alphabetic
character code 111 is alphabetic
character code 112 is alphabetic
character code 113 is alphabetic
character code 114 is alphabetic
character code 115 is alphabetic
character code 116 is alphabetic
character code 117 is alphabetic
character code 118 is alphabetic
character code 119 is alphabetic
character code 120 is alphabetic
character code 121 is alphabetic
character code 122 is alphabetic
===DONE===
