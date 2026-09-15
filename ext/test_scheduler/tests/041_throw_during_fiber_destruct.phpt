--TEST--
Make sure exceptions are rethrown when throwing from fiber destructor — under test_scheduler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$fiber = new Fiber(function() {
    try {
        Fiber::suspend();
    } finally {
        throw new Exception("Exception 2");
    }
});
$fiber->start();
unset($fiber);
throw new Exception("Exception 1");
?>
--EXPECTF--
Fatal error: Uncaught Exception: Exception 1 in %s:%d
Stack trace:
#0 {main}

Next Exception: Exception 2 in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}()
#1 {main}
  thrown in %s on line %d
