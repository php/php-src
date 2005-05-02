--TEST--
math_std_dev() tests
--FILE--
<?php
$a=array(4, 1, 7);
$dev=math_std_dev($a);
var_dump(sprintf("%2.9f", $dev));
var_dump(math_std_dev(array()));
?>
--EXPECTF--
string(11) "2.449489743"

Warning: math_std_dev(): The array has zero elements in %s on line %d
bool(false)