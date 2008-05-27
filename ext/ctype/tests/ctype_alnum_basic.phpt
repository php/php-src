--TEST--
Test ctype_alnum() function : basic functionality
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_alnum(mixed $c)
 * Description: Checks for alphanumeric character(s) 
 * Source code: ext/ctype/ctype.c
 */

echo "*** Testing ctype_alnum() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C"); 

$c1 = 'abcXYZ';
$c2 = ' \t*@';

var_dump(ctype_alnum($c1));
var_dump(ctype_alnum($c2));

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_alnum() : basic functionality ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_alnum_basic.php on line %d
bool(true)
bool(false)

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_alnum_basic.php on line %d
===DONE===
