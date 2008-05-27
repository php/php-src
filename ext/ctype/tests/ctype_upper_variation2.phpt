--TEST--
Test ctype_upper() function : usage variations - different integers
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
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
--EXPECTF--
*** Testing ctype_upper() : usage variations ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_upper_variation2.php on line 13
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
character code 192 is a uppercase character
character code 193 is a uppercase character
character code 194 is a uppercase character
character code 195 is a uppercase character
character code 196 is a uppercase character
character code 197 is a uppercase character
character code 198 is a uppercase character
character code 199 is a uppercase character
character code 200 is a uppercase character
character code 201 is a uppercase character
character code 202 is a uppercase character
character code 203 is a uppercase character
character code 204 is a uppercase character
character code 205 is a uppercase character
character code 206 is a uppercase character
character code 207 is a uppercase character
character code 208 is a uppercase character
character code 209 is a uppercase character
character code 210 is a uppercase character
character code 211 is a uppercase character
character code 212 is a uppercase character
character code 213 is a uppercase character
character code 214 is a uppercase character
character code 216 is a uppercase character
character code 217 is a uppercase character
character code 218 is a uppercase character
character code 219 is a uppercase character
character code 220 is a uppercase character
character code 221 is a uppercase character
character code 222 is a uppercase character

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_upper_variation2.php on line 21
===DONE===
