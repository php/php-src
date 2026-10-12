--TEST--
test_scheduler: gc_collect_cycles() whose destructor iterator gets no finish handler counts the cycle at the next call
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

// The GC coroutine's handler is the first one added, the iterator's the second.
ini_set('test_scheduler.fail_finish_handler', 2);
var_dump(gc_collect_cycles());
var_dump(gc_collect_cycles());
?>
--EXPECT--
destructor
int(0)
int(1)
