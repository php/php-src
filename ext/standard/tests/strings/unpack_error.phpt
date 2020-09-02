--TEST--
Test unpack() function : error conditions
--FILE--
<?php

echo "*** Testing unpack() : error conditions ***\n";

echo "\n-- Testing unpack() function with invalid format character --\n";
$extra_arg = 10;
var_dump(unpack("B", pack("I", 65534)));
?>
--EXPECTF--
*** Testing unpack() : error conditions ***

-- Testing unpack() function with invalid format character --

Warning: unpack(): Invalid format type B in %s on line %d
bool(false)
