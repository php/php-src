--TEST--
Erasure: unbounded T accepts anything (mixed)
--FILE--
<?php
function id<T>(T $x): T { return $x; }
var_dump(id(42));
var_dump(id("hello"));
var_dump(id(null));
var_dump(id([1, 2]));
?>
--EXPECT--
int(42)
string(5) "hello"
NULL
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
