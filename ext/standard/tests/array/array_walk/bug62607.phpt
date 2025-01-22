--TEST--
Bug #62607: array_walk_recursive move internal pointer
--FILE--
<?php
$arr = array('a'=>'b');
echo 'Before -> '.current($arr).PHP_EOL;
array_walk_recursive($arr, function(&$val){});
echo 'After -> '.current($arr);
?>
--EXPECT--
Before -> b
After -> b
