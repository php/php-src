--TEST--
Test ctype_punct() function : basic functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* Prototype  : bool ctype_punct(mixed $c)
 * Description: Checks for any printable character which is not whitespace
 * or an alphanumeric character
 * Source code: ext/ctype/ctype.c
 */

echo "*** Testing ctype_punct() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = '@!$*';
$c2 = 'hello, world!';

var_dump(ctype_punct($c1));
var_dump(ctype_punct($c2));

setlocale(LC_CTYPE, $orig);
?>
===DONE===
--EXPECT--
*** Testing ctype_punct() : basic functionality ***
bool(true)
bool(false)
===DONE===
