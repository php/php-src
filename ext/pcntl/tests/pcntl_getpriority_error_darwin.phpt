--TEST--
pcntl_getpriority() - Wrong mode passed and also for non existing process id provided
--EXTENSIONS--
pcntl
posix
--SKIPIF--
<?php

require_once("pcntl_skipif_user_env_rules.inc");

if (!function_exists('pcntl_getpriority')) {
    die('skip pcntl_getpriority doesn\'t exist');
}

if (PHP_OS !== "Darwin") {
    die("skip This test only runs on Darwin");
}

?>
--FILE--
<?php

try {
    pcntl_getpriority(null, (PRIO_PGRP + PRIO_USER + PRIO_PROCESS + 10));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    pcntl_getpriority(-1, PRIO_DARWIN_THREAD);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    // Different behavior in MacOS than rest of operating systems
    pcntl_getpriority(-1, PRIO_PROCESS);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
pcntl_getpriority(): Argument #2 ($mode) must be one of PRIO_PGRP, PRIO_USER, PRIO_PROCESS or PRIO_DARWIN_THREAD
pcntl_getpriority(): Argument #1 ($process_id) must be 0 (zero) if PRIO_DARWIN_THREAD is provided as second parameter
pcntl_getpriority(): Argument #1 ($process_id) is not a valid process, process group, or user ID
