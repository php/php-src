--TEST--
GH-18509: Dynamic calls to assert() ignore zend.assertions
--INI--
zend.assertions=0
--FILE--
<?php

$c = "assert";

$c(false);

var_dump(array_map(assert(...), [true, true, false]));

?>
--EXPECT--
array(3) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
  [2]=>
  bool(true)
}
