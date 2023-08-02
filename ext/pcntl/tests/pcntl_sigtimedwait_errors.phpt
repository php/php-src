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
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $signals = [-1];
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 2);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $signals = ["not a signal"];
    $signal_no = pcntl_sigtimedwait($signals, $signal_infos, 2);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Unlikely valid signal */
$signals = [2**10];
$signal_no = pcntl_sigtimedwait($signals, $signal_infos, 2);
var_dump($signal_no);
var_dump($signal_infos);
?>
--EXPECTF--
ValueError: pcntl_sigtimedwait(): Argument #1 ($signals) cannot be empty
ValueError: pcntl_sigtimedwait(): Argument #1 ($signals) signals must be between 0 and %d
TypeError: pcntl_sigtimedwait(): Argument #1 ($signals) signals must be of type int, string given

Warning: pcntl_sigtimedwait(): Invalid argument in %s on line %d
bool(false)
NULL
