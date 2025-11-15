--TEST--
BackedEnum::values() preserves declaration order
--FILE--
<?php
enum Order: int {
    case Third = 3;
    case First = 1;
    case Second = 2;
}
var_dump(Order::values());
?>
--EXPECT--
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(1)
  [2]=>
  int(2)
}

