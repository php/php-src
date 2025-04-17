--TEST--
Constant expressions with cast
--FILE--
<?php
class X {
    public int $foo = 3;
}

const T1 = (int) 0.3;
const T2 = (bool) 0.3;
const T3 = (string) [];
const T4 = (object) ["a" => 1];
const T5 = (float) 5;
const T6 = (array) "";
const T7 = (array) var_dump(...);
const T8 = (array) new X;
const T9 = (array) new DateTime;
const T10 = (int) new DateTime;

var_dump(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10);

const C_FLOAT = 0.3;
const C_EMPTY_ARRAY = [];
const C_ARRAY = ["a" => 1];
const C_INT = 5;
const C_EMPTY_STRING = "";
const C_CALLABLE = var_dump(...);
const C_USER_OBJECT = new X;
const C_DATE_TIME = new DateTime;

const T11 = (int) C_FLOAT;
const T12 = (bool) C_FLOAT;
const T13 = (string) C_EMPTY_ARRAY;
const T14 = (object) C_ARRAY;
const T15 = (float) C_INT;
const T16 = (array) C_EMPTY_STRING;
const T17 = (array) C_CALLABLE;
const T18 = (array) C_USER_OBJECT;
const T19 = (array) C_DATE_TIME;
const T20 = (int) C_DATE_TIME;

var_dump(T11, T12, T13, T14, T15, T16, T17, T18, T19, T20);
?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d

Warning: Object of class DateTime could not be converted to int in %s on line %d
int(0)
bool(true)
string(5) "Array"
object(stdClass)#%d (1) {
  ["a"]=>
  int(1)
}
float(5)
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  object(Closure)#%d (2) {
    ["function"]=>
    string(8) "var_dump"
    ["parameter"]=>
    array(2) {
      ["$value"]=>
      string(10) "<required>"
      ["$values"]=>
      string(10) "<optional>"
    }
  }
}
array(1) {
  ["foo"]=>
  int(3)
}
array(3) {
  ["date"]=>
  string(%d) "%s"
  ["timezone_type"]=>
  int(%d)
  ["timezone"]=>
  string(%d) "%s"
}
int(1)

Warning: Array to string conversion in %s on line %d

Warning: Object of class DateTime could not be converted to int in %s on line %d
int(0)
bool(true)
string(5) "Array"
object(stdClass)#%d (1) {
  ["a"]=>
  int(1)
}
float(5)
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  object(Closure)#%d (2) {
    ["function"]=>
    string(8) "var_dump"
    ["parameter"]=>
    array(2) {
      ["$value"]=>
      string(10) "<required>"
      ["$values"]=>
      string(10) "<optional>"
    }
  }
}
array(1) {
  ["foo"]=>
  int(3)
}
array(3) {
  ["date"]=>
  string(%d) "%s"
  ["timezone_type"]=>
  int(%d)
  ["timezone"]=>
  string(%d) "%s"
}
int(1)
