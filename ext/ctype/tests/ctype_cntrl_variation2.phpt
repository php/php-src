--TEST--
Test ctype_cntrl() function : usage variations - different integers
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
 * Pass different integers to ctype_cntrl() to test which character codes are considered
 * valid control characters
 */

echo "*** Testing ctype_cntrl() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C");

for ($i = 0; $i < 256; $i++) {
	if (ctype_cntrl($i)) {
		echo "character code $i is control character\n";
	}
}

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_cntrl() : usage variations ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_cntrl_variation2.php on line 14
character code 0 is control character
character code 1 is control character
character code 2 is control character
character code 3 is control character
character code 4 is control character
character code 5 is control character
character code 6 is control character
character code 7 is control character
character code 8 is control character
character code 9 is control character
character code 10 is control character
character code 11 is control character
character code 12 is control character
character code 13 is control character
character code 14 is control character
character code 15 is control character
character code 16 is control character
character code 17 is control character
character code 18 is control character
character code 19 is control character
character code 20 is control character
character code 21 is control character
character code 22 is control character
character code 23 is control character
character code 24 is control character
character code 25 is control character
character code 26 is control character
character code 27 is control character
character code 28 is control character
character code 29 is control character
character code 30 is control character
character code 31 is control character
character code 127 is control character
character code 128 is control character
character code 129 is control character
character code 130 is control character
character code 131 is control character
character code 132 is control character
character code 133 is control character
character code 134 is control character
character code 135 is control character
character code 136 is control character
character code 137 is control character
character code 138 is control character
character code 139 is control character
character code 140 is control character
character code 141 is control character
character code 142 is control character
character code 143 is control character
character code 144 is control character
character code 145 is control character
character code 146 is control character
character code 147 is control character
character code 148 is control character
character code 149 is control character
character code 150 is control character
character code 151 is control character
character code 152 is control character
character code 153 is control character
character code 154 is control character
character code 155 is control character
character code 156 is control character
character code 157 is control character
character code 158 is control character
character code 159 is control character
character code 173 is control character

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_cntrl_variation2.php on line 22
===DONE===
