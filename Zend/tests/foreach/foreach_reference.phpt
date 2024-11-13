--TEST--
foreach with reference
--FILE--
<?php

$array = ['a', 'b', 'c', 'd'];

foreach ($array as &$a) {
}

var_dump($array);

var_dump(array_values($array));
var_dump($a);

var_dump(array_reverse($array));

?>
--EXPECT--
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  &string(1) "d"
}
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  &string(1) "d"
}
string(1) "d"
array(4) {
  [0]=>
  &string(1) "d"
  [1]=>
  string(1) "c"
  [2]=>
  string(1) "b"
  [3]=>
  string(1) "a"
}
