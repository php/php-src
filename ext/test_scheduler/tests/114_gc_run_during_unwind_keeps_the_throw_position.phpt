--TEST--
test_scheduler: an exception whose unwind waits for the GC run reaches its catch after a destructor that throws and catches
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Node {
    public $self;

    public function __destruct() {
        try {
            throw new LogicException('in destructor');
        } catch (LogicException $e) {
            echo "destructor caught ", $e->getMessage(), "\n";
        }
    }
}

$objects = [];
for ($i = 0; $i < 112000; $i++) {
    $objects[] = new stdClass;
}

$node = new Node();
$node->self = $node;
unset($node);

// Fill the buffer: the root that finds it full queues the GC run.
$i = 0;
while (gc_status()['roots'] < gc_status()['threshold']) {
    $copy = $objects[$i++];
    unset($copy);
}

// Without yielding, one root short of the count past the queued run at which
// the coroutine waits for it: freeing the operand below adds that root.
$queued_at = gc_status()['roots'];
while (gc_status()['roots'] < $queued_at + 99999) {
    $copy = $objects[$i++];
    unset($copy);
}

$tail = [$objects[$i], $objects[$i + 1], $objects[$i + 2]];
$runs = gc_status()['runs'];

try {
    $sum = [...$tail] + 1;
} catch (TypeError $e) {
    echo "caught ", $e->getMessage(), ", collection ran: ", var_export(gc_status()['runs'] > $runs, true), "\n";
}
?>
--EXPECT--
destructor caught in destructor
caught Unsupported operand types: array + int, collection ran: true
