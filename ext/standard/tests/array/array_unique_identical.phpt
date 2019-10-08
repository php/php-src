--TEST--
Test array_unique() with SORT_IDENTICAL flag
--FILE---
<?php

var_dump(SORT_IDENTICAL);
var_dump(array_unique([1, 1, 2, 3], SORT_IDENTICAL));

var_dump(array_unique([1, "1", true], SORT_IDENTICAL));

$obj = new stdClass;
var_dump(array_unique([$obj, $obj], SORT_IDENTICAL));
--EXPECTF--
int(7)
array(3) {
  [0]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(1) "1"
  [2]=>
  bool(true)
}
array(1) {
  [0]=>
  object(stdClass)#1 (0) {
  }
}
