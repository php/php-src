--TEST--
test_scheduler: a GC run queued by a full root buffer as the script ends still collects
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Node {
    public $self;

    public function __destruct() {
        echo "garbage collected, runs: ", gc_status()['runs'], "\n";
    }
}

$objects = [];
for ($i = 0; $i < 12000; $i++) {
    $objects[] = new stdClass;
}

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
echo "script ends, runs: ", gc_status()['runs'], "\n";
?>
--EXPECT--
script ends, runs: 0
garbage collected, runs: 1
