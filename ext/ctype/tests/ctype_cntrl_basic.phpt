--TEST--
Test ctype_cntrl() function : basic functionality
--EXTENSIONS--
ctype
--FILE--
<?php
echo "*** Testing ctype_cntrl() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = "\r\n\t";
$c2 = "Hello, World!\n";

var_dump(ctype_cntrl($c1));
var_dump(ctype_cntrl($c2));

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_cntrl() : basic functionality ***
bool(true)
bool(false)
