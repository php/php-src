--TEST--
test_scheduler: a coroutine cancelled while it waits for the GC run puts its shielded temporaries back as roots
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Node {
    public $self;

    public function __destruct() {
        echo "Node destructed\n";
    }
}

class Canceller {
    public $self;

    public function __destruct() {
        global $target;
        echo "canceller\n";
        TestScheduler\cancel($target);
    }
}

function make(): Node {
    $node = new Node();
    $node->self = $node;
    return $node;
}

$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

$target = TestScheduler\spawn(function () use ($objects) {
    $canceller = new Canceller();
    $canceller->self = $canceller;
    unset($canceller);

    // Two short of a full buffer: the assignment below adds the last root
    // while the result of make() is a live temporary.
    $i = 0;
    while (gc_status()['roots'] < gc_status()['threshold'] - 2) {
        $copy = $objects[$i++];
        unset($copy);
    }

    $last = $objects[$i];
    $result = make() <=> ($last = null);
});

try {
    TestScheduler\await($target);
} catch (Throwable $e) {
    echo "caught ", get_class($e), "\n";
}

// The GC coroutine ends its run before a later coroutine runs.
TestScheduler\await(TestScheduler\spawn(function () {}));
var_dump(gc_collect_cycles());
?>
--EXPECT--
canceller
caught TestScheduler\CancellationError
Node destructed
int(1)
