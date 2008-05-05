--TEST--
Test return type and value for expected input asinh()
--INI--
precision = 14
--FILE--
<?php
/* 
 * proto float asinh(float number)
 * Function is implemented in ext/standard/math.c
*/ 

$file_path = dirname(__FILE__);
require($file_path."/allowed_rounding_error.inc");

echo "asinh  0.52109530549375= ";
var_dump(asinh(0.52109530549375));
if (allowed_rounding_error(asinh(0.52109530549375), 0.5))
{
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

echo "asinh 10.01787492741= ";
var_dump(asinh(10.01787492741));
if (allowed_rounding_error(asinh(10.01787492741), 3.0))
{
	echo "Pass\n";
}
else {
	echo "Fail\n";
}

?>
--EXPECTF--
asinh  0.52109530549375= float(%f)
Pass
asinh 10.01787492741= float(%f)
Pass
