--TEST--
Test ctype_graph() function : basic functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo "*** Testing ctype_graph() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = 'helloWorld!';
$c2 = "Hello, world!\n";

var_dump(ctype_graph($c1));
var_dump(ctype_graph($c2));

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_graph() : basic functionality ***
bool(true)
bool(false)
