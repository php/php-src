--TEST--
test_scheduler: an exception whose unwind fills the root buffer reaches its own catch
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Node {
    public $self;

    public function __destruct() {
        echo "destructor\n";
    }
}

$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

function throw_with_full_buffer(array $objects): void {
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
    throw new Exception('thrown');
}

try {
    throw_with_full_buffer($objects);
} catch (Exception $e) {
    echo "caught ", $e->getMessage(), "\n";
}

var_dump(gc_collect_cycles());
?>
--EXPECT--
caught thrown
destructor
int(1)
