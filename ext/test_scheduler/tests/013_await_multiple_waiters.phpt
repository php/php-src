--TEST--
test_scheduler: several coroutines (the main one included) await the same target
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend, resume, await};

$target = spawn(function () {
    suspend();
    return "shared";
});

$w1 = spawn(fn () => "w1: " . await($target));
$w2 = spawn(fn () => "w2: " . await($target));

spawn(function () use ($target) {
    resume($target);
});

echo "w1 => ", await($w1), "\n"; // main parks in await() too
echo "w2 => ", await($w2), "\n";
echo "==DONE==\n";
?>
--EXPECT--
w1 => w1: shared
w2 => w2: shared
==DONE==
