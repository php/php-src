--TEST--
test_scheduler: a full root buffer queues the GC run, and the coroutine that filled it goes on without suspending
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Node {
    public $self;

    public function __destruct() {
        echo "garbage collected\n";
    }
}

$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

$filler = TestScheduler\spawn(function () use ($objects) {
    $node = new Node();
    $node->self = $node;
    unset($node);

    // Two short of a full buffer: the first release below fills it, the second
    // finds it full.
    $i = 0;
    while (gc_status()['roots'] < gc_status()['threshold'] - 2) {
        $copy = $objects[$i++];
        unset($copy);
    }

    $last = $objects[$i];
    $last = null;
    $last = $objects[$i + 1];
    $last = null;
    echo "filler goes on, runs: ", gc_status()['runs'], "\n";
});

// Queued ahead of the GC coroutine: a filler that waited for the run would let it in first.
$other = TestScheduler\spawn(function () {
    echo "other coroutine\n";
});

TestScheduler\await($filler);
TestScheduler\await($other);
echo "runs: ", gc_status()['runs'], "\n";
?>
--EXPECT--
filler goes on, runs: 0
other coroutine
runs: 1
garbage collected
