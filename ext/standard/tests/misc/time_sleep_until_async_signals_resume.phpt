--TEST--
time_sleep_until() resumes until its deadline after an async signal handler returns
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
$start = hrtime(true);
$calledAfter = null;
pcntl_signal(SIGALRM, static function () use ($start, &$calledAfter): void {
    $calledAfter = hrtime(true) - $start;
});

pcntl_alarm(1);
var_dump(time_sleep_until(microtime(true) + 2));
$elapsed = hrtime(true) - $start;
// The handler ran while sleeping
var_dump($calledAfter !== null && $calledAfter < 1_500_000_000);
// The sleep went on after the handler returned (100ms of tolerance for early wakes)
var_dump($elapsed >= 1_900_000_000);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
