--TEST--
pcntl_signal_dispatch() with an exception pending after an internal function called from a user frame
--EXTENSIONS--
pcntl
zend_test
--FILE--
<?php

pcntl_signal(SIGUSR1, function ($signo) {
    echo "Handler called from ", debug_backtrace()[1]['function'], "()\n";
});

pcntl_async_signals(true);

function test() {
    declare(ticks=1) {
        register_tick_function('zend_test_raise_and_throw', SIGUSR1);
    }
    unregister_tick_function('zend_test_raise_and_throw');
}

test();

?>
--EXPECTF--
Handler called from test()

Fatal error: Uncaught Exception: Exception after raise() in %s:%d
Stack trace:
#0 %s(%d): zend_test_raise_and_throw(%d)
#1 %s(%d): test()
#2 {main}
  thrown in %s on line %d
