--TEST--
math_std_dev()/math_variance tests
--FILE--
<?php
$a=array(4, 1, 7);
$dev=math_std_dev($a);
var_dump(sprintf("%2.9f", $dev));
var_dump(math_std_dev(array()));
$a=array(5,7,8,10,10);
var_dump(math_std_dev($a,1));
echo "---Variance---\n";
$a=array(5,7,8,10,10);
var_dump(math_variance($a));
var_dump(math_variance($a, true));
?>
--EXPECTF--
string(11) "2.449489743"

Warning: math_std_dev(): The array has zero elements in %s on line %d
bool(false)
float(2.1213203435596)
---Variance---
float(3.6)
float(4.5)