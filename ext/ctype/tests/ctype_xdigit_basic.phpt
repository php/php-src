--TEST--
Test ctype_xdigit() function : basic functionality 
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

echo "*** Testing ctype_xdigit() : basic functionality ***\n";
$orig = setlocale(LC_CTYPE, "C"); 

$c1 = 'abcdefABCDEF0123456789';
$c2 = 'face 034';

var_dump(ctype_xdigit($c1));
var_dump(ctype_xdigit($c2));

setlocale(LC_CTYPE, $orig); 
?>
===DONE===
--EXPECTF--
*** Testing ctype_xdigit() : basic functionality ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_xdigit_basic.php on line 8
bool(true)
bool(false)

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_xdigit_basic.php on line 16
===DONE===
