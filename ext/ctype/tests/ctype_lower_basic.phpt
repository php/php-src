--TEST--
Test ctype_lower() function : basic functionality 
--FILE--
<?php
/* Prototype  : bool ctype_lower(mixed $c)
 * Description: Checks for lowercase character(s)  
 * Source code: ext/ctype/ctype.c
 */

echo "*** Testing ctype_lower() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = 'helloworld';
$c2 = "Hello, world!\n";

var_dump(ctype_lower($c1));
var_dump(ctype_lower($c2));

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_lower() : basic functionality ***
bool(true)
bool(false)
===DONE===
