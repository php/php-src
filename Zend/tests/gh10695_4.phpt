--TEST--
GH-10695: Exception handlers are not called twice
--FILE--
<?php
set_exception_handler(function (\Throwable $exception) {
    echo 'Caught: ' . $exception->getMessage() . "\n";
    throw new \Exception('exception handler');
});

throw new \Exception('main');
?>
--EXPECTF--
Caught: main

Fatal error: Uncaught Exception: exception handler in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(Object(Exception))
#1 {main}
  thrown in %s on line %d
