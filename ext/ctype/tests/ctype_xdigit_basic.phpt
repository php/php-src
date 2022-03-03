--TEST--
Test ctype_xdigit() function : basic functionality
--EXTENSIONS--
ctype
--FILE--
<?php
echo "*** Testing ctype_xdigit() : basic functionality ***\n";
$orig = setlocale(LC_CTYPE, "C");

$c1 = 'abcdefABCDEF0123456789';
$c2 = 'face 034';

var_dump(ctype_xdigit($c1));
var_dump(ctype_xdigit($c2));

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_xdigit() : basic functionality ***
bool(true)
bool(false)
