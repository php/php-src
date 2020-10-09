--TEST--
Bug #79393 (Null coalescing operator failing with SplFixedArray)
--FILE--
<?php
$foo = new SplFixedArray(5);
$foo[0] = 'bar1';
$foo[1] = 'bar2';
$foo[2] = 0;
$foo[3] = false;
$foo[4] = '';

var_dump($foo[0] ?? null);
var_dump($foo[1] ?? null);
var_dump($foo[2] ?? null);
var_dump($foo[3] ?? null);
var_dump($foo[4] ?? null);
var_dump($foo[5] ?? null);
?>
--EXPECT--
string(4) "bar1"
string(4) "bar2"
int(0)
bool(false)
string(0) ""
NULL
