--TEST--
GH-10437 (Segfault on suspending a dead fiber)
--FILE--
<?php

set_time_limit(1);

register_shutdown_function(function () {
    Fiber::getCurrent()->suspend();
});

$fiber = new Fiber(function () {
    while (1);
});
$fiber->start();
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s

Fatal error: Uncaught FiberError: Cannot suspend a dead fiber in %s
Stack trace:
#0 %s: Fiber::suspend()
#1 [internal function]: {closure}()
#2 {main}
  thrown in %s
