--TEST--
prev - ensure we cannot pass a temporary
--FILE--
<?php
/*
 * Pass temporary variables to prev() to test behaviour
 */


var_dump(prev(array(1, 2)));
?>
--EXPECTF--
Fatal error: Uncaught Error: prev(): Argument #1 ($array) cannot be passed by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
