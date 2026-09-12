--TEST--
test_scheduler: an exception nobody awaited surfaces as a fatal error
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
TestScheduler\spawn(function () {
    try {
        Fiber::suspend();
    } catch (FiberError $e) {
        echo "caught: ", $e->getMessage(), "\n";
    }
});

TestScheduler\spawn(function () {
    throw new Exception("nobody awaits this");
});

echo "==DONE==\n";
?>
--EXPECTF--
==DONE==
caught: Cannot suspend outside of a fiber

Fatal error: Uncaught Exception: nobody awaits this in %s011_unhandled_exception_surfaces.php:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}()
#1 {main}
  thrown in %s011_unhandled_exception_surfaces.php on line %d
