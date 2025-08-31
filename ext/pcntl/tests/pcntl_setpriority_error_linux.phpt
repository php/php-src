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

if (PHP_OS !== "Linux") {
    die("skip This test only runs on Linux");
}

?>
--FILE--
<?php

pcntl_setpriority(-1000, 1);
pcntl_setpriority(-1000, 0);

?>
--EXPECTF--
Warning: pcntl_setpriority(): Error 1: A process was located, but neither its effective nor real user ID matched the effective user ID of the caller in %s

Warning: pcntl_setpriority(): Error 13: Only a super user may attempt to increase the process priority in %s on line %d
