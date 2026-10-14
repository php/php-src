--TEST--
test_scheduler: awaiting the main coroutine wakes up when the script ends
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend, resume, current};

$main = current();

spawn(function () use ($main) {
    await($main);
    echo "main awaited\n";
});

spawn(function () use ($main) {
    resume($main);
});

suspend();
echo "main done\n";
?>
--EXPECT--
main done
main awaited
