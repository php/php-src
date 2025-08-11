--TEST--
SplPriorityQueue serialization and unserialization
--FILE--
<?php
$queue = new SplPriorityQueue();
$queue->insert('a', 1);
$queue->insert('b', 2);
$queue->insert('c', 3);

echo "Original queue count: " . count($queue) . "\n";

$temp = clone $queue;
$extracted = array();
while (!$temp->isEmpty()) {
    $extracted[] = $temp->extract();
}
echo "Original extraction order: " . implode(', ', $extracted) . "\n";

$serialized = serialize($queue);
$unserialized = unserialize($serialized);

echo "Unserialized queue count: " . count($unserialized) . "\n";

$extracted = array();
while (!$unserialized->isEmpty()) {
    $extracted[] = $unserialized->extract();
}
echo "Unserialized extraction order: " . implode(', ', $extracted) . "\n";

$queue2 = new SplPriorityQueue();
$queue2->setExtractFlags(SplPriorityQueue::EXTR_BOTH);
$queue2->insert('data1', 10);
$queue2->insert('data2', 5);

$serialized2 = serialize($queue2);
$unserialized2 = unserialize($serialized2);

echo "Flags preserved: " . ($unserialized2->getExtractFlags() === SplPriorityQueue::EXTR_BOTH ? "YES" : "NO") . "\n";

$result = $unserialized2->extract();
echo "Extracted with EXTR_BOTH: data=" . $result['data'] . ", priority=" . $result['priority'] . "\n";

?>
--EXPECT--
Original queue count: 3
Original extraction order: c, b, a
Unserialized queue count: 3
Unserialized extraction order: c, b, a
Flags preserved: YES
Extracted with EXTR_BOTH: data=data1, priority=10
