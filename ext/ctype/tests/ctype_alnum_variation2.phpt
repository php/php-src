--TEST--
Test ctype_alnum() function : usage variations - different integers
--FILE--
<?php
/* Prototype  : bool ctype_alnum(mixed $c)
 * Description: Checks for alphanumeric character(s) 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_alnum() to test which character codes are considered
 * valid alphanumeric characters
 */

echo "*** Testing ctype_alnum() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C"); 

for ($i = 0; $i < 256; $i++) {
	if (ctype_alnum($i)) {
		echo "character code $i is alpha numeric\n";
	}
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_alnum() : usage variations ***
character code 48 is alpha numeric
character code 49 is alpha numeric
character code 50 is alpha numeric
character code 51 is alpha numeric
character code 52 is alpha numeric
character code 53 is alpha numeric
character code 54 is alpha numeric
character code 55 is alpha numeric
character code 56 is alpha numeric
character code 57 is alpha numeric
character code 65 is alpha numeric
character code 66 is alpha numeric
character code 67 is alpha numeric
character code 68 is alpha numeric
character code 69 is alpha numeric
character code 70 is alpha numeric
character code 71 is alpha numeric
character code 72 is alpha numeric
character code 73 is alpha numeric
character code 74 is alpha numeric
character code 75 is alpha numeric
character code 76 is alpha numeric
character code 77 is alpha numeric
character code 78 is alpha numeric
character code 79 is alpha numeric
character code 80 is alpha numeric
character code 81 is alpha numeric
character code 82 is alpha numeric
character code 83 is alpha numeric
character code 84 is alpha numeric
character code 85 is alpha numeric
character code 86 is alpha numeric
character code 87 is alpha numeric
character code 88 is alpha numeric
character code 89 is alpha numeric
character code 90 is alpha numeric
character code 97 is alpha numeric
character code 98 is alpha numeric
character code 99 is alpha numeric
character code 100 is alpha numeric
character code 101 is alpha numeric
character code 102 is alpha numeric
character code 103 is alpha numeric
character code 104 is alpha numeric
character code 105 is alpha numeric
character code 106 is alpha numeric
character code 107 is alpha numeric
character code 108 is alpha numeric
character code 109 is alpha numeric
character code 110 is alpha numeric
character code 111 is alpha numeric
character code 112 is alpha numeric
character code 113 is alpha numeric
character code 114 is alpha numeric
character code 115 is alpha numeric
character code 116 is alpha numeric
character code 117 is alpha numeric
character code 118 is alpha numeric
character code 119 is alpha numeric
character code 120 is alpha numeric
character code 121 is alpha numeric
character code 122 is alpha numeric
===DONE===
