--TEST--
Bug 48484 (array_product() always returns 0 for an empty array)
--FILE--
<?php
var_dump(array_product(array()));
?>
--EXPECT--
int(1)
