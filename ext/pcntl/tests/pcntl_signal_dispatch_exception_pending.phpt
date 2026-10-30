--TEST--
pcntl_signal_dispatch() runs the handlers of the signals raised while an internal function ran and then threw
--EXTENSIONS--
pcntl
zend_test
--FILE--
<?php

pcntl_async_signals(true);

pcntl_signal(SIGUSR1, function ($signo) {
    echo "Handler called\n";
});

try {
    zend_test_raise_and_throw(SIGUSR1);
} catch (\Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Handler called
Exception after raise()
