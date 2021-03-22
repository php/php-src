--TEST--
Test ctype_space() function : basic functionality
--EXTENSIONS--
ctype
--FILE--
<?php
echo "*** Testing ctype_space() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = " \t\r\n";
var_dump(ctype_space($c1));

$c2 = "Hello, world!\n";
var_dump(ctype_space($c2));

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_space() : basic functionality ***
bool(true)
bool(false)
