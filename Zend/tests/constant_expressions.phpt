--TEST--
Constant Expressions
--FILE--
<?php
const T_1 = 1 << 1;
const T_2 = 1 / 2;
const T_3 = 1.5 + 1.5;
const T_4 = "foo" . "bar";
const T_5 = (1.5 + 1.5) * 2;
const T_6 = "foo" . 2 . 3 . 4.0;
const T_7 = __LINE__;
const T_8 = <<<ENDOFSTRING
This is a test string
ENDOFSTRING;
const T_9 = ~-1;
const T_10 = (-1?:1) + (0?2:3);

// Test order of operations
const T_11 = 1 + 2 * 3;

// Test for memory leaks
const T_12 = "1" + 2 + "3";

var_dump(T_1);
var_dump(T_2);
var_dump(T_3);
var_dump(T_4);
var_dump(T_5);
var_dump(T_6);
var_dump(T_7);
var_dump(T_8);
var_dump(T_9);
var_dump(T_10);
var_dump(T_11);
var_dump(T_12);
?>
--EXPECT--
int(2)
float(0.5)
float(3)
string(6) "foobar"
float(6)
string(6) "foo234"
int(8)
string(21) "This is a test string"
int(0)
int(2)
int(7)
int(6)
