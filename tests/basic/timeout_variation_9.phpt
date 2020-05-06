--TEST--
Timeout within shutdown function
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

exit(0);
?>
never reached here
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
