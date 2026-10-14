--TEST--
test_scheduler: gc_collect_cycles() whose GC coroutine gets no finish handler collects nothing, and the next call collects
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

$node = new Node();
$node->self = $node;
unset($node);

// The GC coroutine's handler is the first one added.
ini_set('test_scheduler.fail_finish_handler', 1);
var_dump(gc_collect_cycles());
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(0)
destructor
int(1)
