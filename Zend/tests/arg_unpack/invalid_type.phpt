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
Warning: Argument unpacking at parameter 1 requires array or Traversable, null given in %s on line %d
array(0) {
}

Warning: Argument unpacking at parameter 1 requires array or Traversable, integer given in %s on line %d
array(0) {
}

Warning: Argument unpacking at parameter 1 requires array or Traversable, object of type stdClass given in %s on line %d
array(0) {
}

Warning: Argument unpacking at parameter 4 requires array or Traversable, string given in %s on line %d
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

Warning: Argument unpacking at parameter 4 requires array or Traversable, object of type stdClass given in %s on line %d

Warning: Argument unpacking at parameter 4 requires array or Traversable, float given in %s on line %d
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
