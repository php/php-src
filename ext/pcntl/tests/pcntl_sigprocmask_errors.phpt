--TEST--
pcntl_sigprocmask() errors
--EXTENSIONS--
pcntl
--SKIPIF--
<?php if (!function_exists("pcntl_sigprocmask")) die("skip pcntl_sigprocmask() not available"); ?>
--INI--
max_execution_time=0
--FILE--
<?php

/* Invalid mode */
try {
    $signals = [SIGTERM];
    $signal_no = pcntl_sigprocmask(-1, $signals);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    /* This used to return -1 prior to PHP 8.4.0 */
    $signals = [];
    $signal_no = pcntl_sigprocmask(SIG_BLOCK, $signals);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $signals = [0];
    $signal_no = pcntl_sigprocmask(SIG_BLOCK, $signals);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $signals = [-1];
    $signal_no = pcntl_sigprocmask(SIG_BLOCK, $signals);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $signals = ["not a signal"];
    $signal_no = pcntl_sigprocmask(SIG_BLOCK, $signals);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Unlikely valid signal */
try {
    $signals = [2**10];
    $signal_no = pcntl_sigprocmask(SIG_BLOCK, $signals);
    var_dump($signal_no);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
ValueError: pcntl_sigprocmask(): Argument #1 ($mode) must be one of SIG_BLOCK, SIG_UNBLOCK, or SIG_SETMASK
ValueError: pcntl_sigprocmask(): Argument #2 ($signals) cannot be empty
ValueError: pcntl_sigprocmask(): Argument #2 ($signals) signals must be between 1 and %d
ValueError: pcntl_sigprocmask(): Argument #2 ($signals) signals must be between 1 and %d
TypeError: pcntl_sigprocmask(): Argument #2 ($signals) signals must be of type int, string given
ValueError: pcntl_sigprocmask(): Argument #2 ($signals) signals must be between 1 and %d
