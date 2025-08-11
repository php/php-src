--TEST--
SplMaxHeap serialization and unserialization
--FILE--
<?php
$heap = new SplMaxHeap();
$heap->insert(50);
$heap->insert(20);
$heap->insert(80);
$heap->insert(10);
$heap->insert(30);

echo "Original heap count: " . count($heap) . "\n";

$temp = clone $heap;
$extracted = array();
while (!$temp->isEmpty()) {
    $extracted[] = $temp->extract();
}
echo "Original extraction order: " . implode(', ', $extracted) . "\n";

$serialized = serialize($heap);
$unserialized = unserialize($serialized);

echo "Unserialized heap count: " . count($unserialized) . "\n";

$extracted = array();
while (!$unserialized->isEmpty()) {
    $extracted[] = $unserialized->extract();
}
echo "Unserialized extraction order: " . implode(', ', $extracted) . "\n";

?>
--EXPECT--
Original heap count: 5
Original extraction order: 80, 50, 30, 20, 10
Unserialized heap count: 5
Unserialized extraction order: 80, 50, 30, 20, 10
