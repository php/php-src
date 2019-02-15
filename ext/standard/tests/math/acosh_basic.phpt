--TEST--
Test return type and value for expected input acosh()
--INI--
precision = 14
--FILE--
<?php
/*
 * proto float acosh(float number)
 * Function is implemented in ext/standard/math.c
*/

$file_path = dirname(__FILE__);
require($file_path."/allowed_rounding_error.inc");

echo "acosh  1.1276259652064= ";
var_dump(acosh(1.1276259652064));
if (allowed_rounding_error(acosh(1.1276259652064), 0.5))
{
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "acosh 10.067661995778= ";
var_dump(acosh(10.067661995778));
if (allowed_rounding_error(acosh(10.067661995778), 3.0))
{
	echo "Pass\n";
}
else {
	echo "Fail\n";
}


?>
--EXPECTF--
acosh  1.1276259652064= float(%f)
Pass
acosh 10.067661995778= float(%f)
Pass
