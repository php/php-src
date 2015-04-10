--TEST--
Bug #69413: Warning in array_count_values() about array values not being string/int
--FILE--
<?php

$array = ['a', 'b'];
$ref =& $array[0];

var_dump(array_count_values($array));

?>
--EXPECT--
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(1)
}
