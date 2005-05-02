--TEST--
math_std_dev() tests
--FILE--
<?php
$a=array(4, 1, 7);
$dev=math_std_dev($a);
var_dump(sprintf("%2.9f", $dev));
?>
--EXPECT--
string(11) "2.449489743"