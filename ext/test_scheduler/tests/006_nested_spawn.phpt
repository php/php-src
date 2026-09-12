--TEST--
test_scheduler: a coroutine spawns another one
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\spawn;

spawn(function () {
    echo "outer: start\n";
    spawn(function () { echo "inner\n"; });
    echo "outer: end\n";
});

echo "main: end\n";
?>
--EXPECT--
main: end
outer: start
outer: end
inner
