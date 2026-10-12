--TEST--
test_scheduler: a coroutine that adds roots far past the queued GC run without yielding waits for the run
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
$filler = TestScheduler\spawn(function () {
    // Each cycle adds a root and is garbage: nothing here yields.
    $max_roots = 0;
    for ($i = 0; $i < 120000; $i++) {
        $node = new stdClass;
        $node->self = $node;
        unset($node);
        $max_roots = max($max_roots, gc_status()['roots']);
    }

    $status = gc_status();
    echo "runs before yielding: {$status['runs']}\n";
    echo "roots stayed under 110 001 + 1: ", var_export($max_roots <= 110002, true), "\n";
});

TestScheduler\await($filler);
?>
--EXPECT--
runs before yielding: 1
roots stayed under 110 001 + 1: true
