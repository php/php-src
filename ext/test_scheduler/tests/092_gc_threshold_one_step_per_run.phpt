--TEST--
test_scheduler: coroutines waiting for one GC run raise the threshold by one step
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await};

// Live objects: each release of a copy makes a possible root that is not garbage.
$waiter_objects = [];
for ($i = 0; $i < 100; $i++) {
    $waiter_objects[] = new stdClass;
}

$main_objects = [];
for ($i = 0; $i < 12000; $i++) {
    $main_objects[] = new stdClass;
}

// Queued ahead of the GC coroutine, each one releases its object while the
// buffer is full and waits for the same run as main.
foreach ($waiter_objects as $object) {
    $last = spawn(function ($object) {}, $object);
}

foreach ($main_objects as $object) {
    $copy = $object;
    unset($copy);
}

await($last);

$status = gc_status();
echo "runs: {$status['runs']}\n";
echo "threshold: {$status['threshold']}\n";
?>
--EXPECT--
runs: 1
threshold: 20001
