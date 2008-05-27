--TEST--
Test ctype_graph() function : basic functionality 
--SKIPIF--
<?php
if( ini_get("unicode.semantics") == "1") {
    die('skip do not run when unicode on');
}
?>
--FILE--
<?php
/* Prototype  : bool ctype_graph(mixed $c)
 * Description: Checks for any printable character(s) except space 
 * Source code: ext/ctype/ctype.c
 */

echo "*** Testing ctype_graph() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = 'helloWorld!';
$c2 = "Hello, world!\n";

var_dump(ctype_graph($c1));
var_dump(ctype_graph($c2));

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_graph() : basic functionality ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_graph_basic.php on line 9
bool(true)
bool(false)

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %sctype_graph_basic.php on line 17
===DONE===
