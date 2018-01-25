--TEST--
Test ctype_print() function : usage variations - different integers
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_print(mixed $c)
 * Description: Checks for printable character(s)
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_print() to test which character codes are considered
 * valid printable characters
 */

echo "*** Testing ctype_print() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
	if (ctype_print($i)) {
		echo "character code $i is a printable character\n";
	}
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_print() : usage variations ***
character code 32 is a printable character
character code 33 is a printable character
character code 34 is a printable character
character code 35 is a printable character
character code 36 is a printable character
character code 37 is a printable character
character code 38 is a printable character
character code 39 is a printable character
character code 40 is a printable character
character code 41 is a printable character
character code 42 is a printable character
character code 43 is a printable character
character code 44 is a printable character
character code 45 is a printable character
character code 46 is a printable character
character code 47 is a printable character
character code 48 is a printable character
character code 49 is a printable character
character code 50 is a printable character
character code 51 is a printable character
character code 52 is a printable character
character code 53 is a printable character
character code 54 is a printable character
character code 55 is a printable character
character code 56 is a printable character
character code 57 is a printable character
character code 58 is a printable character
character code 59 is a printable character
character code 60 is a printable character
character code 61 is a printable character
character code 62 is a printable character
character code 63 is a printable character
character code 64 is a printable character
character code 65 is a printable character
character code 66 is a printable character
character code 67 is a printable character
character code 68 is a printable character
character code 69 is a printable character
character code 70 is a printable character
character code 71 is a printable character
character code 72 is a printable character
character code 73 is a printable character
character code 74 is a printable character
character code 75 is a printable character
character code 76 is a printable character
character code 77 is a printable character
character code 78 is a printable character
character code 79 is a printable character
character code 80 is a printable character
character code 81 is a printable character
character code 82 is a printable character
character code 83 is a printable character
character code 84 is a printable character
character code 85 is a printable character
character code 86 is a printable character
character code 87 is a printable character
character code 88 is a printable character
character code 89 is a printable character
character code 90 is a printable character
character code 91 is a printable character
character code 92 is a printable character
character code 93 is a printable character
character code 94 is a printable character
character code 95 is a printable character
character code 96 is a printable character
character code 97 is a printable character
character code 98 is a printable character
character code 99 is a printable character
character code 100 is a printable character
character code 101 is a printable character
character code 102 is a printable character
character code 103 is a printable character
character code 104 is a printable character
character code 105 is a printable character
character code 106 is a printable character
character code 107 is a printable character
character code 108 is a printable character
character code 109 is a printable character
character code 110 is a printable character
character code 111 is a printable character
character code 112 is a printable character
character code 113 is a printable character
character code 114 is a printable character
character code 115 is a printable character
character code 116 is a printable character
character code 117 is a printable character
character code 118 is a printable character
character code 119 is a printable character
character code 120 is a printable character
character code 121 is a printable character
character code 122 is a printable character
character code 123 is a printable character
character code 124 is a printable character
character code 125 is a printable character
character code 126 is a printable character
===DONE===
