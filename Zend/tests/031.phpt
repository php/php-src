--TEST--
Testing array with '[]' passed as argument by value
--FILE--
<?php

function test($var) { }
test($arr[]);

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use [] for reading in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
