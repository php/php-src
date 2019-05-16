--TEST--
SPL: DoublyLinkedList: Queues
--FILE--
<?php
$queue = new SplQueue();
// errors
try {
    $queue->dequeue();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
try {
    $queue->shift();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

// data consistency
$a = 2;
$queue->enqueue($a);
echo $queue->dequeue()."\n";

// peakable
$queue->enqueue(1);
$queue->enqueue(2);
echo $queue->top()."\n";

// iterable
foreach ($queue as $elem) {
    echo "[$elem]\n";
}

// countable
$queue->enqueue(NULL);
$queue->enqueue(NULL);
echo count($queue)."\n";
echo $queue->count()."\n";
var_dump($queue->dequeue());
var_dump($queue->dequeue());

// clonable
$queue->enqueue(2);
$queue_clone = clone $queue;
$queue_clone->dequeue();
echo count($queue)."\n";
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Exception: Can't shift from an empty datastructure
Exception: Can't shift from an empty datastructure
2
2
[1]
[2]
4
4
int(1)
int(2)
3
===DONE===
