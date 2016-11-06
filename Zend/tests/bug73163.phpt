--TEST--
Bug #73163 (PHP hangs if error handler throws while accessing undef const in default value)
--FILE--
<?php

function doSomething(string $value = UNDEFINED) {
}

set_error_handler(function($errno, $errstr) {
    throw new Exception($errstr);
});

doSomething();

?>
--EXPECTF--
Fatal error: Uncaught Exception: Use of undefined constant UNDEFINED - assumed 'UNDEFINED' in %s:%d
Stack trace:
#0 %s(%d): {closure}(%s)
#1 %s(%d): doSomething()
#2 {main}
  thrown in %s on line %d
