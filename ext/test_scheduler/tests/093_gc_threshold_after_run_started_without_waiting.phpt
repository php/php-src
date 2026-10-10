--TEST--
test_scheduler: a full root buffer where switching is blocked raises the threshold after the run
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
declare(ticks=1);

use function TestScheduler\{spawn, await};

$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

$ticked = false;

// A tick function cannot wait: the full buffer only starts the GC coroutine.
register_tick_function(function () use (&$ticked, $objects) {
    if ($ticked) {
        return;
    }

    $ticked = true;
    foreach ($objects as $object) {
        $copy = $object;
        unset($copy);
    }
});

echo "threshold after tick: ", gc_status()['threshold'], "\n";

// Queued behind the GC coroutine.
await(spawn(function () {}));

$status = gc_status();
echo "runs: {$status['runs']}\n";
echo "threshold after run: {$status['threshold']}\n";
?>
--EXPECT--
threshold after tick: 10001
runs: 1
threshold after run: 20001
