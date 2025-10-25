--TEST--
Bug GH-20262 (array_unique() with SORT_REGULAR fails to remove duplicates with mixed strings)
--FILE--
<?php

// Original bug report case
$units = ['5', '10', '3A', '5'];
var_dump(array_unique($units, SORT_REGULAR));

?>
--EXPECT--
array(3) {
  [0]=>
  string(1) "5"
  [1]=>
  string(2) "10"
  [2]=>
  string(2) "3A"
}
