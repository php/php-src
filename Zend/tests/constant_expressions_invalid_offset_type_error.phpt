--TEST--
Can't use arrays as key for constant array
--FILE--
<?php

const C1 = 1; // force dynamic evaluation
const C2 = [C1, [] => 1];

?>
--EXPECTF--
Fatal error: Uncaught Error: Illegal offset type in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
