--TEST--
Don't create FETCH_DIM_R with UNUSED op2
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

// Order matters
test($arr[]);
function test($arg) {}

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use [] for reading in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
