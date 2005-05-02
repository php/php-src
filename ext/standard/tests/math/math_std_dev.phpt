--TEST--
math_std_dev()/math_variance tests
--FILE--
<?php
$a=array(4, 1, 7);
$dev=math_std_dev($a);
var_dump(sprintf("%2.9f", $dev));
var_dump(math_std_dev(array()));
echo "---Variance---\n";
$a=array(5,7,8,10,10);
var_dump(math_variance($a));
?>
--EXPECTF--
string(11) "2.449489743"

Warning: math_std_dev(): The array has zero elements in %s on line %d
bool(false)
---Variance---
float(3.6)