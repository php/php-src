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

if (PHP_OS == "Darwin") {
    die("skip This test is not for Darwin");
}

?>
--FILE--
<?php

try {
    $result = pcntl_setpriority(0, null, (PRIO_PGRP + PRIO_USER + PRIO_PROCESS + 10));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

pcntl_setpriority(0, -123);

?>
--EXPECTF--
pcntl_setpriority(): Argument #3 ($mode) must be one of PRIO_PGRP, PRIO_USER, or PRIO_PROCESS

Warning: pcntl_setpriority(): Error 3: No process was located using the given parameters in %s
