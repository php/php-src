--TEST--
test_scheduler: an exit whose unwind runs the GC stays an exit when the waiting coroutine is cancelled meanwhile
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
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

function leave(array $objects): void {
    $node = new Node();
    $node->self = $node;
    unset($node);

    // Two short of a full buffer: the unwind below adds the last root.
    $i = 0;
    while (gc_status()['roots'] < gc_status()['threshold'] - 2) {
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
