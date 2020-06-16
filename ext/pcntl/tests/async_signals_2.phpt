--TEST--
Async signals in zend_call_function
--SKIPIF--
<?php
if (!extension_loaded("pcntl")) print "skip";
if (getenv("SKIP_SLOW_TESTS")) print "skip slow test";
?>
--FILE--
<?php

pcntl_async_signals(1);
pcntl_signal(SIGALRM, function($signo) {
    throw new Exception("Alarm!");
});

pcntl_alarm(1);
try {
    array_map(
        'time_nanosleep',
        array_fill(0, 360, 1),
        array_fill(0, 360, 0)
    );
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Alarm!
