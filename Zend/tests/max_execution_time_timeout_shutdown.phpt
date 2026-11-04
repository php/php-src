--TEST--
max_execution_time timeout during a shutdown function
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
max_execution_time=1
--FILE--
<?php

register_shutdown_function(function () {
    while (true) {}
});

?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded %s
