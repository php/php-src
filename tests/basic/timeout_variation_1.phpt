--TEST--
Timeout within function
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (PHP_OS_FAMILY !== "Windows") die("skip Windows only test");
?>
--FILE--
<?php

set_time_limit(1);

sleep(1);
sleep(1);

?>
never reached here
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
