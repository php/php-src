--TEST--
test_scheduler: a full root buffer whose GC coroutine cannot be created raises the threshold at once
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await};

$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

await(spawn(function () use ($objects) {
    // The GC coroutine is the next coroutine created.
    ini_set('test_scheduler.fail_new_coroutine', 1);

    foreach ($objects as $object) {
        $copy = $object;
        unset($copy);
    }

    ini_set('test_scheduler.fail_new_coroutine', 0);
}));

$status = gc_status();
echo "runs: {$status['runs']}\n";
echo "threshold: {$status['threshold']}\n";
?>
--EXPECT--
runs: 0
threshold: 20001
