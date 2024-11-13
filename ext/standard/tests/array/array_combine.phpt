--TEST--
basic array_combine test
--FILE--
<?php
$array1 = array('green', 'red', 'yellow');
$array2 = array('1', '2', '3');
$array3 = array(0, 1, 2);
$array4 = array(TRUE, FALSE, NULL);
$a = array_combine($array1, $array1);
$b = array_combine($array1, $array2);
$c = array_combine($array1, $array3);
$d = array_combine($array1, $array4);
$e = array_combine($array2, $array1);
$f = array_combine($array2, $array2);
$g = array_combine($array2, $array3);
$h = array_combine($array2, $array4);
$i = array_combine($array3, $array1);
$j = array_combine($array3, $array2);
$k = array_combine($array3, $array3);
$l = array_combine($array3, $array4);
$m = array_combine($array4, $array1);
$n = array_combine($array4, $array2);
$o = array_combine($array4, $array3);
$p = array_combine($array4, $array4);

$letters = range('a', 'p');
foreach ($letters as $letter) {
    var_dump($$letter);
}
?>
--EXPECT--
array(3) {
  ["green"]=>
  string(5) "green"
  ["red"]=>
  string(3) "red"
  ["yellow"]=>
  string(6) "yellow"
}
array(3) {
  ["green"]=>
  string(1) "1"
  ["red"]=>
  string(1) "2"
  ["yellow"]=>
  string(1) "3"
}
array(3) {
  ["green"]=>
  int(0)
  ["red"]=>
  int(1)
  ["yellow"]=>
  int(2)
}
array(3) {
  ["green"]=>
  bool(true)
  ["red"]=>
  bool(false)
  ["yellow"]=>
  NULL
}
array(3) {
  [1]=>
  string(5) "green"
  [2]=>
  string(3) "red"
  [3]=>
  string(6) "yellow"
}
array(3) {
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
  [3]=>
  string(1) "3"
}
array(3) {
  [1]=>
  int(0)
  [2]=>
  int(1)
  [3]=>
  int(2)
}
array(3) {
  [1]=>
  bool(true)
  [2]=>
  bool(false)
  [3]=>
  NULL
}
array(3) {
  [0]=>
  string(5) "green"
  [1]=>
  string(3) "red"
  [2]=>
  string(6) "yellow"
}
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
array(3) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  NULL
}
array(2) {
  [1]=>
  string(5) "green"
  [""]=>
  string(6) "yellow"
}
array(2) {
  [1]=>
  string(1) "1"
  [""]=>
  string(1) "3"
}
array(2) {
  [1]=>
  int(0)
  [""]=>
  int(2)
}
array(2) {
  [1]=>
  bool(true)
  [""]=>
  NULL
}
