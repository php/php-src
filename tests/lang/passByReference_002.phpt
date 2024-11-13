--TEST--
Attempt to pass a constant by reference
--FILE--
<?php

function f(&$arg1)
{
    var_dump($arg1++);
}

f(2);

?>
--EXPECTF--
Fatal error: Uncaught Error: f(): Argument #1 ($arg1) could not be passed by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
