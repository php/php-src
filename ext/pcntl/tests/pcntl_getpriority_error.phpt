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

if (PHP_OS == "Darwin") {
    die("skip This test is not for Darwin");
}

?>
--FILE--
<?php

try {
    pcntl_getpriority(null, PRIO_PGRP + PRIO_USER + PRIO_PROCESS + 10);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

// Different behavior in MacOS than rest of operating systems
pcntl_getpriority(-1, PRIO_PROCESS);

?>
--EXPECTF--
pcntl_getpriority(): Argument #2 ($mode) must be one of PRIO_PGRP, PRIO_USER, or PRIO_PROCESS

Warning: pcntl_getpriority(): Error %d: No process was located using the given parameters in %s
