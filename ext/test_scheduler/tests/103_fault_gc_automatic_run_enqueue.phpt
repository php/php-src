--TEST--
test_scheduler: a full root buffer whose GC coroutine cannot be queued raises the threshold, and the refusal is thrown nowhere
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
    // The GC coroutine is the next coroutine queued.
    ini_set('test_scheduler.fail_enqueue', 1);

    foreach ($objects as $object) {
        $copy = $object;
        unset($copy);
    }

    echo "roots added\n";
}));

$status = gc_status();
echo "runs: {$status['runs']}\n";
echo "threshold: {$status['threshold']}\n";
?>
--EXPECT--
roots added
runs: 0
threshold: 20001
