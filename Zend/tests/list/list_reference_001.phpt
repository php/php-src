--TEST--
"Reference Unpacking - General" list()
--FILE--
<?php
$arr = array(1, array(2));
list(&$a, list(&$b)) = $arr;
var_dump($a, $b);
var_dump($arr);

$arr = array(1, array(2));
list($a, &$b) = $arr;
var_dump($arr);

$arr = array(1, array(2));
[&$a, [&$b]] = $arr;
var_dump($a, $b);
var_dump($arr);

$arr = array(1, array(2));
[&$a, [&$b], &$c] = $arr;
var_dump($a, $b, $c);
var_dump($arr);

$arr = array("one" => 1, "two" => array(2));
["one" => &$a, "two" => [&$b], "three" => &$c] = $arr;
var_dump($a, $b, $c);
var_dump($arr);
?>
--EXPECT--
int(1)
int(2)
array(2) {
  [0]=>
  &int(1)
  [1]=>
  array(1) {
    [0]=>
    &int(2)
  }
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  &array(1) {
    [0]=>
    int(2)
  }
}
int(1)
int(2)
array(2) {
  [0]=>
  &int(1)
  [1]=>
  array(1) {
    [0]=>
    &int(2)
  }
}
int(1)
int(2)
NULL
array(3) {
  [0]=>
  &int(1)
  [1]=>
  array(1) {
    [0]=>
    &int(2)
  }
  [2]=>
  &NULL
}
int(1)
int(2)
NULL
array(3) {
  ["one"]=>
  &int(1)
  ["two"]=>
  array(1) {
    [0]=>
    &int(2)
  }
  ["three"]=>
  &NULL
}
