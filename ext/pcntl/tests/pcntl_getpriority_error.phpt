--TEST--
pcntl_getpriority() - Wrong mode passed and also for non existing process id provided
--EXTENSIONS--
pcntl
--SKIPIF--
<?php

if (!function_exists('pcntl_getpriority')) {
    die('skip pcntl_getpriority doesn\'t exist');
}
?>
--FILE--
<?php

try {
    pcntl_getpriority(null, (PRIO_PGRP - PRIO_USER - PRIO_PROCESS));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

pcntl_getpriority(-123);

?>
--EXPECTF--
pcntl_getpriority(): Argument #2 ($mode) must be one of PRIO_PGRP, PRIO_USER, or PRIO_PROCESS

Warning: pcntl_getpriority(): Error 3: No process was located using the given parameters in %s
