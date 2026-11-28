--TEST--
time_sleep_until() leaves manually dispatched signals pending
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('time_sleep_until')) die('skip time_sleep_until() not available');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow test');
?>
--FILE--
<?php
pcntl_async_signals(false);
$called = false;
pcntl_signal(SIGALRM, static function () use (&$called): void {
    $called = true;
});

pcntl_alarm(1);
var_dump(time_sleep_until(microtime(true) + 2));
var_dump($called);
pcntl_signal_dispatch();
var_dump($called);
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
