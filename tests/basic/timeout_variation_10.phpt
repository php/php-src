--TEST--
Timeout within shutdown function, variation
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (PHP_OS_FAMILY !== "Windows") die("skip Windows only test");
?>
--FILE--
<?php

set_time_limit(1);

register_shutdown_function("sleep", 1);
register_shutdown_function("sleep", 1);

?>
shutdown happens after here
--EXPECTF--
shutdown happens after here

Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
