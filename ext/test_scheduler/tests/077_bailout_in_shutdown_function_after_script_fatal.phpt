--TEST--
test_scheduler: a fatal error in a shutdown function after a fatal error in the script also ends the coroutines it queued
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
    eval('function in_shutdown() {}');
    eval('function in_shutdown() {}');
});
echo "main\n";
eval('function in_script() {}');
eval('function in_script() {}');
?>
--EXPECTF--
main

Fatal error: Cannot redeclare function in_script() (previously declared in %s : eval()'d code:1) in %s : eval()'d code on line 1

Fatal error: Cannot redeclare function in_shutdown() (previously declared in %s : eval()'d code:1) in %s : eval()'d code on line 1
