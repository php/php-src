--TEST--
Test ctype_space() function : basic functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_space(mixed $c)
 * Description: Checks for whitespace character(s)
 * Source code: ext/ctype/ctype.c
 */

echo "*** Testing ctype_space() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = " \t\r\n";
var_dump(ctype_space($c1));

$c2 = "Hello, world!\n";
var_dump(ctype_space($c2));

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECTF--
*** Testing ctype_space() : basic functionality ***
bool(true)
bool(false)
===DONE===
