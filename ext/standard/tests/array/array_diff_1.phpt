--TEST--
Test array_diff when non-array is passed
--FILE--
<?php
//-=-=-=-=-
$a = array();
$b = 3;
$c = array(5);
array_diff($a, $b, $c);
//-=-=-=-=-=-
echo "OK!";
?>
--EXPECTF--
Warning: array_diff(): Expected parameter 2 to be an array, int given in %s
OK!
