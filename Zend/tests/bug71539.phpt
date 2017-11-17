--TEST--
Bug #71539 (Memory error on $arr[$a] =& $arr[$b] if RHS rehashes)
--FILE--
<?php
$array = [];
$array[0] =& $array[''];
$array[0] = 42;
var_dump($array);
?>
--EXPECT--
array(2) {
  [""]=>
  &int(42)
  [0]=>
  &int(42)
}
