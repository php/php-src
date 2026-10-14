--TEST--
test_scheduler: exit() inside a scheduler coroutine ends the request, scheduler handles it itself
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

use function TestScheduler\spawn;

/* The scheduler runs the bodies of its own coroutines, so it observes the
 * unwind itself and no engine notification is raised. */
spawn(function () {
    echo "coroutine: running\n";
    exit(5);
});

echo "main: end\n";
?>
--EXPECT--
main: end
coroutine: running
