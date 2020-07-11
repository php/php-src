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
Fatal error: Uncaught Error: Cannot pass parameter 1 by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
