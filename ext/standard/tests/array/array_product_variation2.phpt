--TEST--
Test array_product() function : variation
--FILE--
<?php
echo "*** Testing array_product() : variations ***\n";

echo "\n-- Testing array_product() function with a keyed array array --\n";
var_dump( array_product(array("bob" => 2, "janet" => 5)) );
?>
--EXPECT--
*** Testing array_product() : variations ***

-- Testing array_product() function with a keyed array array --
int(10)
