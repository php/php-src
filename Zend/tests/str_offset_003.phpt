--TEST--
string offset 003
--FILE--
<?php
// Test negative string offsets

function foo($x) {
    var_dump($x);
}

$str = "abcdef";
var_dump($str[-10]);
var_dump($str[-3]);
var_dump($str[2][-2]);
var_dump($str[2][-1]);

foo($str[-10]);
foo($str[-3]);
foo($str[2][-2]);
foo($str[2][-1]);
?>
--EXPECTF--
Warning: Uninitialized string offset -10 in %s on line %d
string(0) ""
string(1) "d"

Warning: Uninitialized string offset -2 in %s on line %d
string(0) ""
string(1) "c"

Warning: Uninitialized string offset -10 in %s on line %d
string(0) ""
string(1) "d"

Warning: Uninitialized string offset -2 in %s on line %d
string(0) ""
string(1) "c"
