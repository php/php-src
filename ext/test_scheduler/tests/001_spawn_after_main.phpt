--TEST--
test_scheduler: coroutines spawned by the script run when the script ends
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\spawn;

echo "main: start\n";

spawn(function () { echo "A\n"; });
spawn(function () { echo "B\n"; });

echo "main: end\n";
?>
--EXPECT--
main: start
main: end
A
B
