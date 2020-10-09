--TEST--
Test ctype_upper() function : basic functionality
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo "*** Testing ctype_upper() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = 'HELLOWORLD';
$c2 = "Hello, World!\n";

var_dump(ctype_upper($c1));
var_dump(ctype_upper($c2));

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_upper() : basic functionality ***
bool(true)
bool(false)
