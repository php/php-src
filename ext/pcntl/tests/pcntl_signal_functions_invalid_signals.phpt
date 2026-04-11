--TEST--
pcntl_sigprocmask(), pcntl_sigwaitinfo(), and pcntl_sigtimedwait() properly throw on invalid signals
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists("pcntl_sigprocmask")) die("skip pcntl_sigprocmask() not available");
if (!function_exists("pcntl_sigwaitinfo")) die("skip pcntl_sigwaitinfo() not available");
if (!function_exists("pcntl_sigtimedwait")) die("skip pcntl_sigtimedwait() not available");
?>
--INI--
max_execution_time=0
--FILE--
<?php

try {
    pcntl_sigprocmask(SIG_BLOCK, ["not_a_signal"]);
} catch (TypeError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    pcntl_sigprocmask(SIG_BLOCK, [0]);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    pcntl_sigprocmask(SIG_BLOCK, []);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    pcntl_sigwaitinfo(["not_a_signal"]);
} catch (TypeError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    pcntl_sigwaitinfo([0]);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    pcntl_sigwaitinfo([]);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    pcntl_sigtimedwait(["not_a_signal"], $info, 1);
} catch (TypeError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    pcntl_sigtimedwait([0], $info, 1);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

try {
    pcntl_sigtimedwait([], $info, 1);
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECTF--
pcntl_sigprocmask(): Argument #2 ($signals) signals must be of type int, string given
pcntl_sigprocmask(): Argument #2 ($signals) signals must be between 1 and %d
pcntl_sigprocmask(): Argument #2 ($signals) must not be empty
pcntl_sigwaitinfo(): Argument #1 ($signals) signals must be of type int, string given
pcntl_sigwaitinfo(): Argument #1 ($signals) signals must be between 1 and %d
pcntl_sigwaitinfo(): Argument #1 ($signals) must not be empty
pcntl_sigtimedwait(): Argument #1 ($signals) signals must be of type int, string given
pcntl_sigtimedwait(): Argument #1 ($signals) signals must be between 1 and %d
pcntl_sigtimedwait(): Argument #1 ($signals) must not be empty
