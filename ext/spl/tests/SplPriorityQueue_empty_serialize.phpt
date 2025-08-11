--TEST--
SplPriorityQueue empty queue serialization and unserialization
--FILE--
<?php
$queue = new SplPriorityQueue();

echo "Empty queue count: " . count($queue) . "\n";
echo "Empty queue isEmpty: " . ($queue->isEmpty() ? "true" : "false") . "\n";

$serialized = serialize($queue);
$unserialized = unserialize($serialized);

echo "Unserialized empty queue count: " . count($unserialized) . "\n";
echo "Unserialized empty queue isEmpty: " . ($unserialized->isEmpty() ? "true" : "false") . "\n";

$unserialized->insert('test', 1);
echo "After insert count: " . count($unserialized) . "\n";
echo "Extract from unserialized: " . $unserialized->extract() . "\n";

?>
--EXPECT--
Empty queue count: 0
Empty queue isEmpty: true
Unserialized empty queue count: 0
Unserialized empty queue isEmpty: true
After insert count: 1
Extract from unserialized: test
