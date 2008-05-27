--TEST--
Test ctype_upper() function : basic functionality 
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

echo "*** Testing ctype_upper() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = 'HELLOWORLD';
$c2 = "Hello, World!\n";

var_dump(ctype_upper($c1));
var_dump(ctype_upper($c2));

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_upper() : basic functionality ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_upper_basic.php on line 9
bool(true)
bool(false)

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_upper_basic.php on line 17
===DONE===
