--TEST--
Test ctype_alpha() function : basic functionality
--EXTENSIONS--
ctype
--FILE--
<?php
echo "*** Testing ctype_alpha() : basic functionality ***\n";

$orig = setlocale(LC_CTYPE, "C");

$c1 = 'abcXYZ';
$c2 = "Hello, World!";

var_dump(ctype_alpha($c1));
var_dump(ctype_alpha($c2));

setlocale(LC_CTYPE, $orig);
?>
--EXPECT--
*** Testing ctype_alpha() : basic functionality ***
bool(true)
bool(false)
