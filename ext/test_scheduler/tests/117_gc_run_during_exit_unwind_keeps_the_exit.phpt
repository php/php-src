--TEST--
test_scheduler: an exit whose unwind waits for the GC run stays an exit when the waiting coroutine is cancelled meanwhile
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Node {
    public $self;

    public function __destruct() {
        global $target;
        echo "destructor cancels\n";
        TestScheduler\cancel($target);
    }
}

$objects = [];
for ($i = 0; $i < 112000; $i++) {
    $objects[] = new stdClass;
}

function leave(array $objects): void {
    $node = new Node();
    $node->self = $node;
    unset($node);

    // Fill the buffer: the root that finds it full queues the GC run.
    $i = 0;
    while (gc_status()['roots'] < gc_status()['threshold']) {
        $copy = $objects[$i++];
        unset($copy);
    }

    // Without yielding, one root short of the count past the queued run at
    // which the coroutine waits for it: the statement below adds that root.
    $queued_at = gc_status()['roots'];
    while (gc_status()['roots'] < $queued_at + 99999) {
        $copy = $objects[$i++];
        unset($copy);
    }

    $last = $objects[$i];
    exit("exit\n");
}

$target = TestScheduler\spawn(function () use ($objects) {
    try {
        leave($objects);
    } catch (Throwable $e) {
        echo "caught ", get_class($e), "\n";
    }

    echo "after exit\n";
});

TestScheduler\await($target);
echo "main after await\n";
?>
--EXPECT--
exit
destructor cancels
main after await
