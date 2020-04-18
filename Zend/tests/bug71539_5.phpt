--TEST--
Bug #71539.5 (Memory error on $arr[$a] =& $arr[$b] if RHS rehashes)
--FILE--
<?php
$array = [];
$array[''][0] =& $array[0];
$array[0] = 42;
var_dump($array);
?>
--EXPECT--
array(2) {
  [0]=>
  &int(42)
  [""]=>
  array(1) {
    [0]=>
    &int(42)
  }
}
