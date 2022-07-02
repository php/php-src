--TEST--
pcntl_getpriority() - Wrong process identifier
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
    pcntl_getpriority(null, 42);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
pcntl_getpriority(): Argument #2 ($mode) must be one of PRIO_PGRP, PRIO_USER, or PRIO_PROCESS
