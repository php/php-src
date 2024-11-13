--TEST--
errmsg: only variables can be passed by reference
--FILE--
<?php

function foo (&$var) {
}

foo(1);

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: foo(): Argument #1 ($var) could not be passed by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
