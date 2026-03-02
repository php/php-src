--TEST--
Bug GH-9244 (Segfault with array_multisort + array_shift)
--FILE--
<?php
$items = ['foo' => 1, 'bar' => 2];
$order = [4, 3];
array_multisort($order, $items);
var_dump(array_shift($items));
?>
--EXPECT--
int(2)
