--TEST--
Can't use arrays as key for constant array
--FILE--
<?php

const C1 = 1; // force dynamic evaluation
const C2 = [C1, [] => 1];

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot access offset of type array on array in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
