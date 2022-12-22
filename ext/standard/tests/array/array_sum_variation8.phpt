--TEST--
Test array_sum() function: ressources in array
--FILE--
<?php
$input = [10, STDERR /* Should get casted to 3 as an integer */];
var_dump(array_sum($input));
?>
--EXPECT--
int(13)
