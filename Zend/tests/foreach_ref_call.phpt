--TEST--
Foreach by reference with call in foreach variable
--FILE--
<?php

function &returnAppend(&$array) {
    return $array[];
}

$array1 = [1, 2, 3];
$array2 = [];
foreach ($array1 as &returnAppend($array2)['x']) {}
var_dump($array1, $array2);

// The reference unwrap will perform an additional call to returnAppend().

?>
--EXPECT--
array(3) {
  [0]=>
  &int(1)
  [1]=>
  &int(2)
  [2]=>
  &int(3)
}
array(4) {
  [0]=>
  array(1) {
    ["x"]=>
    &int(1)
  }
  [1]=>
  array(1) {
    ["x"]=>
    &int(2)
  }
  [2]=>
  array(1) {
    ["x"]=>
    &int(3)
  }
  [3]=>
  NULL
}
