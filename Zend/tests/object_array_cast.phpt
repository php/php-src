--TEST--
(object) (array) and (array) (object) casts
--FILE--
<?php

$arr = [1, 2, 3];
var_dump((object) (array) $arr);
var_dump($arr);

$obj = (object) [1, 2, 3];
var_dump((array) (object) $obj);
var_dump($obj);

?>
--EXPECT--
object(stdClass)#1 (3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
object(stdClass)#1 (3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
