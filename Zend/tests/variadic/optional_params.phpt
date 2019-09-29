--TEST--
Optional parameter before variadic parameter
--FILE--
<?php

function f($reqParam, $optParam = null, ...$params) {
    var_dump($reqParam, $optParam, $params);
}
 
f(1);
f(1, 2);
f(1, 2, 3);
f(1, 2, 3, 4);
f(1, 2, 3, 4, 5);

?>
--EXPECT--
int(1)
NULL
array(0) {
}
int(1)
int(2)
array(0) {
}
int(1)
int(2)
array(1) {
  [0]=>
  int(3)
}
int(1)
int(2)
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
int(1)
int(2)
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(4)
  [2]=>
  int(5)
}
