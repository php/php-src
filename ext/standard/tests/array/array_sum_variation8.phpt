--TEST--
Test array_sum() function: ressources in array
--FILE--
<?php
$input = [10, STDERR /* Should get casted to 3 as an integer */];
var_dump(array_sum($input));
?>
--EXPECTF--
Warning: array_sum(): Addition is not supported on type resource in %s on line %d
int(13)
