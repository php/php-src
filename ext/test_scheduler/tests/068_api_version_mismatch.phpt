--TEST--
test_scheduler: a provider built for another Async API version is refused
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
test_scheduler.api_version=999
--FILE--
<?php
echo "started\n";

try {
    TestScheduler\spawn(fn() => 1);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: The module test_scheduler cannot register an Async scheduler: it was built for Async API version 999, the core provides %d in Unknown on line 0
started
The scheduler is not running
