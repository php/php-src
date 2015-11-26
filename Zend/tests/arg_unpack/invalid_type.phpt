--TEST--
Only arrays and Traversables can be unpacked
--FILE--
<?php

function test(...$args) {
    var_dump($args);
}

test(...null);
test(...42);
test(...new stdClass);

test(1, 2, 3, ..."foo", ...[4, 5]);
test(1, 2, 3, ...new StdClass, ...3.14, ...[4, 5]);

?>
--EXPECTF--
Warning: Only arrays and Traversables can be unpacked in %s on line %d
array(0) {
}

Warning: Only arrays and Traversables can be unpacked in %s on line %d
array(0) {
}

Warning: Only arrays and Traversables can be unpacked in %s on line %d
array(0) {
}

Warning: Only arrays and Traversables can be unpacked in %s on line %d
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}

Warning: Only arrays and Traversables can be unpacked in %s on line %d

Warning: Only arrays and Traversables can be unpacked in %s on line %d
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
