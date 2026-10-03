--TEST--
test_scheduler: exit() in a shutdown function ends the coroutines it queued, as the bailout it is
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
register_shutdown_function(function () {
    TestScheduler\spawn(function () {
        echo "coroutine ran\n";
    });
    echo "shutdown function\n";
    exit(0);
});
echo "main\n";
?>
--EXPECT--
main
shutdown function
