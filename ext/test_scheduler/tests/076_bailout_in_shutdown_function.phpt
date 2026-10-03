--TEST--
test_scheduler: a fatal error in a shutdown function ends the coroutines it queued, as a fatal error in the script does
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
    eval('function redeclared() {}');
    eval('function redeclared() {}');
});
echo "main\n";
?>
--EXPECTF--
main

Fatal error: Cannot redeclare function redeclared() (previously declared in %s : eval()'d code:1) in %s : eval()'d code on line 1
