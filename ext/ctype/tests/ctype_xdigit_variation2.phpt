--TEST--
Test ctype_xdigit() function : usage variations - different integers
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_xdigit(mixed $c)
 * Description: Checks for character(s) representing a hexadecimal digit 
 * Source code: ext/ctype/ctype.c
 */

/*
 * Pass different integers to ctype_xdigit() to test which character codes are considered
 * valid hexadecimal 'digits'
 */

echo "*** Testing ctype_xdigit() : usage variations ***\n";

$orig = setlocale(LC_CTYPE, "C"); 

for($c = 1; $c < 256; $c++) {
	if (ctype_xdigit($c)) {
		echo "character code $c is a hexadecimal 'digit'\n";
	}
}

setlocale(LC_CTYPE, $orig); 
?>
===DONE===
--EXPECTF--
*** Testing ctype_xdigit() : usage variations ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_xdigit_variation2.php on line 14
character code 48 is a hexadecimal 'digit'
character code 49 is a hexadecimal 'digit'
character code 50 is a hexadecimal 'digit'
character code 51 is a hexadecimal 'digit'
character code 52 is a hexadecimal 'digit'
character code 53 is a hexadecimal 'digit'
character code 54 is a hexadecimal 'digit'
character code 55 is a hexadecimal 'digit'
character code 56 is a hexadecimal 'digit'
character code 57 is a hexadecimal 'digit'
character code 65 is a hexadecimal 'digit'
character code 66 is a hexadecimal 'digit'
character code 67 is a hexadecimal 'digit'
character code 68 is a hexadecimal 'digit'
character code 69 is a hexadecimal 'digit'
character code 70 is a hexadecimal 'digit'
character code 97 is a hexadecimal 'digit'
character code 98 is a hexadecimal 'digit'
character code 99 is a hexadecimal 'digit'
character code 100 is a hexadecimal 'digit'
character code 101 is a hexadecimal 'digit'
character code 102 is a hexadecimal 'digit'

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_xdigit_variation2.php on line 22
===DONE===
