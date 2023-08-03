--TEST--
pcntl_sigtimedwait() errors
--EXTENSIONS--
pcntl
--SKIPIF--
<?php if (!function_exists("pcntl_sigtimedwait")) die("skip pcntl_sigtimedwait() not available"); ?>
--INI--
max_execution_time=0
--FILE--
<?php

try {
    /* This used to return -1 prior to PHP 8.3.0 */
    $signals = [];
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 2);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Invalid signal numbers */
try {
    $signals = [0];
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 2);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $signals = [-1];
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 2);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $signals = [2**10];
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 2);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Invalid signal type */
try {
    $signals = ["not a signal"];
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 2);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Invalid (nano)second values */
$signals = [SIGTERM];
try {
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, -1);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 1, -1);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 1, PHP_INT_MAX);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $signal_no = var_dump(pcntl_sigtimedwait([SIGTERM], $signal_infos, 0, 0));
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
ValueError: pcntl_sigtimedwait(): Argument #1 ($signals) cannot be empty
ValueError: pcntl_sigtimedwait(): Argument #1 ($signals) signals must be between 1 and %d
ValueError: pcntl_sigtimedwait(): Argument #1 ($signals) signals must be between 1 and %d
ValueError: pcntl_sigtimedwait(): Argument #1 ($signals) signals must be between 1 and %d
TypeError: pcntl_sigtimedwait(): Argument #1 ($signals) signals must be of type int, string given
ValueError: pcntl_sigtimedwait(): Argument #3 ($seconds) must be greater than or equal to 0
ValueError: pcntl_sigtimedwait(): Argument #4 ($nanoseconds) must be between 0 and 1e9
ValueError: pcntl_sigtimedwait(): Argument #4 ($nanoseconds) must be between 0 and 1e9
ValueError: pcntl_sigtimedwait(): At least one of argument #3 ($seconds) or argument #4 ($nanoseconds) must be greater than 0
