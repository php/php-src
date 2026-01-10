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
const T_11 = 1 && 0;
const T_12 = 1 and 1;
const T_13 = 0 || 0;
const T_14 = 1 or 0;
const T_15 = 1 xor 1;
const T_16 = 1 xor 0;
const T_17 = 1 < 0;
const T_18 = 0 <= 0;
const T_19 = 1 > 0;
const T_20 = 1 >= 0;
const T_21 = 1 === 1;
const T_22 = 1 !== 1;
const T_23 = 0 != "0";
const T_24 = 1 == "1";

// Test order of operations
const T_25 = 1 + 2 * 3;

// Test for memory leaks
const T_26 = "1" + 2 + "3";

// Allow T_POW
const T_27 = 2 ** 3;

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
var_dump(T_13);
var_dump(T_14);
var_dump(T_15);
var_dump(T_16);
var_dump(T_17);
var_dump(T_18);
var_dump(T_19);
var_dump(T_20);
var_dump(T_21);
var_dump(T_22);
var_dump(T_23);
var_dump(T_24);
var_dump(T_25);
var_dump(T_26);
var_dump(T_27);
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
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
int(7)
int(6)
int(8)
