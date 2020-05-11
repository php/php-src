--TEST--
Nested iteration of SplFixedArray using foreach loops
--FILE--
<?php

$array = SplFixedArray::fromArray([0, 1]);

foreach ($array as $value1) {
  foreach ($array as $value2) {
    echo "$value1 $value2\n";
  }
}

?>
--EXPECT--
0 0
0 1
1 0
1 1
