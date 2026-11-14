--TEST--
time_sleep_until() dispatches async signals and keeps the original deadline
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('time_sleep_until')) die('skip time_sleep_until() not available');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
?>
--FILE--
<?php
pcntl_async_signals(true);
$deadline = microtime(true) + 2;
$calledBeforeDeadline = false;
pcntl_signal(SIGALRM, static function () use ($deadline, &$calledBeforeDeadline): void {
    $calledBeforeDeadline = microtime(true) < $deadline;
    time_sleep_until($deadline + 0.1);
});

pcntl_alarm(1);
var_dump(time_sleep_until($deadline));
var_dump($calledBeforeDeadline);
var_dump(microtime(true) >= $deadline);
var_dump(microtime(true) < $deadline + 0.8);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
