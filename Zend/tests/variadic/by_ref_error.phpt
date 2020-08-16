--TEST--
By-ref variadics enforce the reference
--FILE--
<?php

function test(&... $args) { }

test(1);

?>
--EXPECTF--
Fatal error: Uncaught Error: test(): Argument #1 cannot be passed by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
