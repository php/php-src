--TEST--
Backed enums: values() returns ints
--FILE--
<?php

enum Priority: int {
    case Low = 1;
    case Medium = 5;
    case High = 10;
}

var_dump(Priority::values());

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(10)
}

