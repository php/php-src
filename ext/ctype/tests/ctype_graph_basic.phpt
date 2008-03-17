--TEST--
Test ctype_graph() function : basic functionality 
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
bool(true)
bool(false)
===DONE===
