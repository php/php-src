--TEST--
test_scheduler: a launch that gives no main coroutine ends the request before the script runs
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
test_scheduler.fail_launch=1
--FILE--
<?php
echo "not reached\n";
?>
--EXPECTF--
Fatal error: The scheduler failed to launch: no main coroutine for the top-level script in %s on line %d
