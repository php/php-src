--TEST--
test_scheduler: the main flow suspends and a coroutine resumes it
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend, resume, current};

$main = current();

$a = spawn(function () use ($main) {
    echo "A: resuming main\n";
    resume($main);
    echo "A: done\n";
});

echo "main: suspending\n";
suspend();
echo "main: resumed, A finished: ", var_export($a->isFinished(), true), "\n";
?>
--EXPECT--
main: suspending
A: resuming main
A: done
main: resumed, A finished: true
