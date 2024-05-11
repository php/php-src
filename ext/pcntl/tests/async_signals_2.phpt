--TEST--
Async signals in zend_call_function
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
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
