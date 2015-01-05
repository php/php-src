--TEST--
Basic variadic function
--FILE--
<?php

function test1(... $args) {
    var_dump($args);
}

test1();
test1(1);
test1(1, 2, 3);

function test2($arg1, $arg2, ...$args) {
    var_dump($arg1, $arg2, $args);
}

test2(1, 2);
test2(1, 2, 3);
test2(1, 2, 3, 4, 5);

?>
--EXPECT--
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
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
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(4)
  [2]=>
  int(5)
}
