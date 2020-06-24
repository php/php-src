--TEST--
Test array_product() function : variation
--FILE--
<?php
echo "*** Testing array_product() : variations ***\n";

echo "\n-- Testing array_product() function with a very large array --\n";

$array = array();

for ($i = 0; $i < 999; $i++) {
  $array[] = 999;
}

var_dump( array_product($array) );
?>
--EXPECT--
*** Testing array_product() : variations ***

-- Testing array_product() function with a very large array --
float(INF)
