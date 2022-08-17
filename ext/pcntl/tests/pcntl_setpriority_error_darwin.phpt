--TEST--
pcntl_setpriority() - Check for errors
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php

require_once("pcntl_skipif_user_env_rules.inc");

if (!function_exists('pcntl_setpriority')) {
    die('skip pcntl_setpriority doesn\'t exist');
}

if (PHP_OS !== "Darwin") {
    die("skip This test only runs on Darwin");
}

?>
--FILE--
<?php

try {
    pcntl_setpriority(0, null, (PRIO_PGRP + PRIO_USER + PRIO_PROCESS + 10));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    pcntl_setpriority(0, -1, PRIO_DARWIN_THREAD);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    pcntl_setpriority(0, -123);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

pcntl_setpriority(-1000, 1);

?>
--EXPECTF--
pcntl_setpriority(): Argument #3 ($mode) must be one of PRIO_PGRP, PRIO_USER, PRIO_PROCESS or PRIO_DARWIN_THREAD
pcntl_setpriority(): Argument #2 ($process_id) must be 0 (zero) if PRIO_DARWIN_THREAD is provided as second parameter
pcntl_setpriority(): Argument #2 ($process_id) is not a valid process, process group, or user ID

Warning: pcntl_setpriority(): Error 1: A process was located, but neither its effective nor real user ID matched the effective user ID of the caller in %s
