--TEST--
string offset 001
--FILE--
<?php
// Test positive or null string offsets

function foo($x) {
	var_dump($x);
}

$str = "abc";
var_dump($str[0]);
var_dump($str[1]);
var_dump($str[2]);
var_dump($str[3]);
var_dump($str[1][0]);
var_dump($str[2][1]);

foo($str[0]);
foo($str[1]);
foo($str[2]);
foo($str[3]);
foo($str[1][0]);
foo($str[2][1]);
?>
--EXPECTF--
string(1) "a"
string(1) "b"
string(1) "c"

Notice: Uninitialized string offset: 3 in %sstr_offset_001.php on line %d
string(0) ""
string(1) "b"

Notice: Uninitialized string offset: 1 in %sstr_offset_001.php on line %d
string(0) ""
string(1) "a"
string(1) "b"
string(1) "c"

Notice: Uninitialized string offset: 3 in %sstr_offset_001.php on line %d
string(0) ""
string(1) "b"

Notice: Uninitialized string offset: 1 in %sstr_offset_001.php on line %d
string(0) ""
