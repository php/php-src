--TEST--
Bug #71539.5 (Memory error on $arr[$a] =& $arr[$b] if RHS rehashes)
--FILE--
<?php
$array = [];
$array['']->prop =& $array[0];
$array[0] = 42;
var_dump($array);
?>
--EXPECTF--
Notice: Undefined index:  in %s on line %d

Warning: Attempt to modify property 'prop' of non-object in %s on line %d
array(1) {
  [0]=>
  int(42)
}
