--TEST--
Test array_product() function: ressources in array
--FILE--
<?php
$input = [10, STDERR /* Should get casted to 3 as an integer */];
var_dump(array_product($input));
?>
--EXPECT--
int(30)
