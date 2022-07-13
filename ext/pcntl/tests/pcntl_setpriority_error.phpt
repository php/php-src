--TEST--
pcntl_setpriority() - check for errors
--EXTENSIONS--
pcntl
--SKIPIF--
<?php

if (!function_exists('pcntl_setpriority')) {
    die('skip pcntl_setpriority doesn\'t exist');
}
?>
--FILE--
<?php

$result = true;
try {
    $result = pcntl_setpriority(0, null, (PRIO_PGRP - PRIO_USER - PRIO_PROCESS));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump($result);

pcntl_setpriority(0, -123);

pcntl_setpriority(0, 1);
?>
--EXPECTF--
pcntl_setpriority(): Argument #3 ($mode) must be one of PRIO_PGRP, PRIO_USER, or PRIO_PROCESS
bool(true)

Warning: pcntl_setpriority(): Error 3: No process was located using the given parameters in %s

Warning: pcntl_setpriority(): Error 1: A process was located, but neither its effective nor real user ID matched the effective user ID of the caller in %s
