--TEST--
An async signal handler can exit during time_sleep_until()
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
pcntl_signal(SIGALRM, static function () use ($deadline): void {
    var_dump(microtime(true) < $deadline);
    exit;
});

pcntl_alarm(1);
time_sleep_until($deadline);
echo "Sleep completed\n";
?>
--EXPECT--
bool(true)
