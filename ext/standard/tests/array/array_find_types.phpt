--TEST--
basic array_find test
--FILE--
<?php

$stdClass = new stdClass();
$stdClass->a = "1";

$array = [
    ...[0, 1, 2, -1, 034, 0X4A],
    ...[0.0, 1.2, 1.2e3, 1.2e-3],
    ...['value1', "value2", '', " ", ""],
    ...[true, false, TRUE, FALSE],
    ...[null, NULL],
    ...[1 => 'one', 'zero' => 0, -2 => "value"],
    ...["one" => 1, 5 => "float", true => 1, "" => 'empty'],
    ...[1 => 'one', 2, "key" => 'value'],
    ...[new stdClass(), $stdClass, (object)['b' => 2]],
    ...[[], [1, 2, 3], [-1 => 1, -2 => 2]],
    ...[@$undefined_var]
];

echo "*** Dumping each key value pair. ***". PHP_EOL;
var_dump(array_find($array, function ($value, $key) {
    var_dump($key, $value);

    return false;
}));

echo "*** Dumping only first key value pair. ***". PHP_EOL;
var_dump(array_find($array, function ($value, $key) {
    var_dump($key, $value);

    return true;
}));

echo "*** Returning each value. ***". PHP_EOL;
foreach (array_keys($array) as $key) {
    var_dump(array_find($array, function ($value, $arrayKey) use ($key) {
        return $arrayKey === $key;
    }));
}
?>
--EXPECT--
*** Dumping each key value pair. ***
int(0)
int(0)
int(1)
int(1)
int(2)
int(2)
int(3)
int(-1)
int(4)
int(28)
int(5)
int(74)
int(6)
float(0)
int(7)
float(1.2)
int(8)
float(1200)
int(9)
float(0.0012)
int(10)
string(6) "value1"
int(11)
string(6) "value2"
int(12)
string(0) ""
int(13)
string(1) " "
int(14)
string(0) ""
int(15)
bool(true)
int(16)
bool(false)
int(17)
bool(true)
int(18)
bool(false)
int(19)
NULL
int(20)
NULL
int(21)
string(3) "one"
string(4) "zero"
int(0)
int(22)
string(5) "value"
string(3) "one"
int(1)
int(23)
string(5) "float"
int(24)
int(1)
string(0) ""
string(5) "empty"
int(25)
string(3) "one"
int(26)
int(2)
string(3) "key"
string(5) "value"
int(27)
object(stdClass)#2 (0) {
}
int(28)
object(stdClass)#1 (1) {
  ["a"]=>
  string(1) "1"
}
int(29)
object(stdClass)#3 (1) {
  ["b"]=>
  int(2)
}
int(30)
array(0) {
}
int(31)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
int(32)
array(2) {
  [-1]=>
  int(1)
  [-2]=>
  int(2)
}
int(33)
NULL
NULL
*** Dumping only first key value pair. ***
int(0)
int(0)
int(0)
*** Returning each value. ***
int(0)
int(1)
int(2)
int(-1)
int(28)
int(74)
float(0)
float(1.2)
float(1200)
float(0.0012)
string(6) "value1"
string(6) "value2"
string(0) ""
string(1) " "
string(0) ""
bool(true)
bool(false)
bool(true)
bool(false)
NULL
NULL
string(3) "one"
int(0)
string(5) "value"
int(1)
string(5) "float"
int(1)
string(5) "empty"
string(3) "one"
int(2)
string(5) "value"
object(stdClass)#2 (0) {
}
object(stdClass)#1 (1) {
  ["a"]=>
  string(1) "1"
}
object(stdClass)#3 (1) {
  ["b"]=>
  int(2)
}
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  [-1]=>
  int(1)
  [-2]=>
  int(2)
}
NULL
