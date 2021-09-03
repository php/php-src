--TEST--
Adding a function object return type
--FILE--
<?php

function a() : object {
    return 12345;
}
a();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: a(): Return value must be of type object, int returned in %s:%d
Stack trace:
#0 %s(6): a()
#1 {main}
  thrown in %s on line 4
