--TEST--
Vector toArray()
--FILE--
<?php

$vec = new Vector([new stdClass()]);
var_dump($vec->toArray());
var_dump($vec->toArray());
$vec = new Vector([]);
var_dump($vec->toArray());
var_dump($vec->toArray());
$vec->setSize(2, strtoupper('test'));
var_dump($vec->toArray());
var_dump($vec->toArray());
var_dump(Vector::__set_state([])->toArray());
var_dump(Vector::__set_state([(object)[]])->toArray());
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
array(0) {
}
array(0) {
}
array(2) {
  [0]=>
  string(4) "TEST"
  [1]=>
  string(4) "TEST"
}
array(2) {
  [0]=>
  string(4) "TEST"
  [1]=>
  string(4) "TEST"
}
array(0) {
}
array(1) {
  [0]=>
  object(stdClass)#1 (0) {
  }
}