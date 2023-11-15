--TEST--
pcntl_sigwaitinfo() errors
--EXTENSIONS--
pcntl
--SKIPIF--
<?php if (!function_exists("pcntl_sigwaitinfo")) die("skip pcntl_sigtimedwait() not available"); ?>
--INI--
max_execution_time=0
--FILE--
<?php

try {
    /* This used to return -1 prior to PHP 8.4.0 */
    $signals = [];
    $signal_no = pcntl_sigwaitinfo($signals, $signal_infos);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $signals = [0];
    $signal_no = pcntl_sigwaitinfo($signals, $signal_infos);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $signals = [-1];
    $signal_no = pcntl_sigwaitinfo($signals, $signal_infos);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $signals = ["not a signal"];
    $signal_no = pcntl_sigwaitinfo($signals, $signal_infos);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Unlikely valid signal */
try {
    $signals = [2**10];
    $signal_no = pcntl_sigwaitinfo($signals, $signal_infos);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
ValueError: pcntl_sigwaitinfo(): Argument #1 ($signals) cannot be empty
ValueError: pcntl_sigwaitinfo(): Argument #1 ($signals) signals must be between 1 and %d
ValueError: pcntl_sigwaitinfo(): Argument #1 ($signals) signals must be between 1 and %d
TypeError: pcntl_sigwaitinfo(): Argument #1 ($signals) signals must be of type int, string given
ValueError: pcntl_sigwaitinfo(): Argument #1 ($signals) signals must be between 1 and %d
