--TEST--
Test array_product() function with empty array
--FILE--
<?php
$input = [];
var_dump(array_product($input));
?>
--EXPECTF--
Deprecated: array_product(): Passing an empty array is deprecated in %s on line %d
int(1)
