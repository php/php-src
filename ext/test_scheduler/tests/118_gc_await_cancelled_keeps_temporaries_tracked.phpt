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

$target = TestScheduler\spawn(function () {
    $canceller = new Canceller();
    $canceller->self = $canceller;
    unset($canceller);

    // The result of make() is a live temporary while the call waits for the run.
    $result = make() <=> gc_collect_cycles();
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
