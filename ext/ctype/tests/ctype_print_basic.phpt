--TEST--
Test ctype_print() function : basic functionality
--EXTENSIONS--
ctype
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
--EXPECTF--
*** Testing ctype_print() : basic functionality ***
bool(true)

Deprecated: ctype_print(): Argument of type null will be interpreted as string in the future in %s on line %d
bool(false)
