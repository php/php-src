--TEST--
Test ctype_digit() function : basic functionality
--FILE--
<?php
/* Prototype  : bool ctype_digit(mixed $c)
 * Description: Checks for numeric character(s) 
 * Source code: ext/ctype/ctype.c
 */

echo "*** Testing ctype_digit() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = '1234';
$c2 = 'abc123';

var_dump(ctype_digit($c1));
var_dump(ctype_digit($c2));

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_digit() : basic functionality ***
bool(true)
bool(false)
===DONE===
