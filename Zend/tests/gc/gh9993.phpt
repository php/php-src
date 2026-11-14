--TEST--
GH-9993: GC threshold follows the cost of traversing a large live graph
--SKIPIF--
<?php
// gc_threshold is global state
if (getenv('SKIP_REPEAT')) die('skip Not repeatable');
?>
--INI--
zend.enable_gc=1
memory_limit=256M
--FILE--
<?php

class Node {
    public $owner;
    public $data;
}

const LIVE_NODES = 200000;

function work(?stdClass $container, int $iterations): void {
    for ($i = 0; $i < $iterations; $i++) {
        $a = new Node;
        $b = new Node;
        $a->data = $b;
        $b->data = $a;
        unset($a, $b);

        if ($container) {
            // Adds a node of the live graph to the root buffer, so that the
            // next GC run traverses the whole live graph.
            $node = $container->nodes[$i % LIVE_NODES];
            unset($node);
        }
    }
}

$container = new stdClass;
$container->nodes = [];
for ($i = 0; $i < LIVE_NODES; $i++) {
    $node = new Node;
    $node->owner = $container;
    $container->nodes[] = $node;
}
unset($node);

$before = gc_status();
work($container, 500000);
$after = gc_status();

echo "Live graph:\n";
var_dump($after['threshold'] >= LIVE_NODES / 8);
var_dump($after['buffer_size'] >= $after['threshold']);
// Without the cost rule the GC runs every 10000 roots, i.e. 140 times.
var_dump($after['runs'] - $before['runs'] < 100);

unset($container);
work(null, 500000);
$after = gc_status();

echo "No live graph:\n";
var_dump($after['threshold']);
?>
--EXPECT--
Live graph:
bool(true)
bool(true)
bool(true)
No live graph:
int(10001)
