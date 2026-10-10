--TEST--
An exception thrown from a destroyed fiber's finally block reaches the end of the request — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/throw-during-fiber-destruct.phpt. Under a scheduler the
fiber runs as a coroutine, and destroying the unfinished fiber cancels it: the
graceful exit is delivered at its Fiber::suspend() when the scheduler next
runs the coroutine, here after the script. unset() therefore throws nothing:
the script throws "Exception 1", and the fiber's finally block throws
"Exception 2" in the after-script run. test_scheduler reports the exceptions
nobody caught there as one chain, the later one carrying the earlier as its
previous.
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
