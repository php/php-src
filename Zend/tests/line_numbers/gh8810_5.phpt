--TEST--
GH-8810: Fix reported line number of multi-line assert call
--FILE--
<?php

assert(
    false,
);

?>
--EXPECTF--
Fatal error: Uncaught AssertionError: assert(false) in %s:3
Stack trace:
#0 %s(3): assert(false, 'assert(false)')
#1 {main}
  thrown in %s on line 3
