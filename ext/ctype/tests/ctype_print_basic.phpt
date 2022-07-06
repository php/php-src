--TEST--
Test ctype_print() function : basic functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo "*** Testing ctype_print() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = "Hello, World!";
$c2 = null;

var_dump(ctype_print($c1));
var_dump(ctype_print($c2));

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_print() : basic functionality ***
bool(true)
bool(false)
