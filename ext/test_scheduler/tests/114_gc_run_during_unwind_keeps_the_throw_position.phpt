--TEST--
test_scheduler: an exception whose unwind runs the GC reaches its catch after a destructor that throws and catches
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
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

$node = new Node();
$node->self = $node;
unset($node);

// Two short of a full buffer: freeing the operand below adds the last roots.
$i = 0;
while (gc_status()['roots'] < gc_status()['threshold'] - 2) {
    $copy = $objects[$i++];
    unset($copy);
}

$tail = [$objects[$i], $objects[$i + 1], $objects[$i + 2]];

try {
    $sum = [...$tail] + 1;
} catch (TypeError $e) {
    echo "caught ", $e->getMessage(), "\n";
}
?>
--EXPECT--
destructor caught in destructor
caught Unsupported operand types: array + int
