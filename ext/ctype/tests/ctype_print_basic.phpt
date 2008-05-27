--TEST--
Test ctype_print() function : basic functionality 
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_print(mixed $c)
 * Description: Checks for printable character(s) 
 * Source code: ext/ctype/ctype.c
 */

echo "*** Testing ctype_print() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = "Hello, World!";
$c2 = null;

var_dump(ctype_print($c1));
var_dump(ctype_print($c2));

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_print() : basic functionality ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_print_basic.php on line 9
bool(true)
bool(false)

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_print_basic.php on line 17
===DONE===
