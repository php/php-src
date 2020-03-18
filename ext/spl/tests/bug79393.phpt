--TEST--
Bug #79393 (Null coalescing operator failing with SplFixedArray)
--FILE--
<?php
$foo = new SplFixedArray(2);
$foo[0] = 'bar1';
$foo[1] = 'bar2';

var_dump($foo[0] ?? null);
var_dump($foo[1] ?? null);
var_dump($foo[2] ?? null);
?>
--EXPECT--
string(4) "bar1"
string(4) "bar2"
NULL
