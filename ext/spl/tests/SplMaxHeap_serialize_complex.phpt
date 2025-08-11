--TEST--
SplMaxHeap serialization with arrays and complex data
--FILE--
<?php
$heap = new SplMaxHeap();

$heap->insert(100);
$heap->insert(50);
$heap->insert(75);

echo "Original heap count: " . count($heap) . "\n";

$serialized = serialize($heap);
$unserialized = unserialize($serialized);

echo "Unserialized heap count: " . count($unserialized) . "\n";

$extracted = [];
while (!$unserialized->isEmpty()) {
    $extracted[] = $unserialized->extract();
}
echo "Extraction order: " . implode(', ', $extracted) . "\n";

$heap2 = new SplMaxHeap();
$heap2->insert(['type' => 'array1', 'value' => 10]);
$heap2->insert(['type' => 'array2', 'value' => 20]);
$heap2->insert(['type' => 'array3', 'value' => 5]);

echo "\nArray heap count: " . count($heap2) . "\n";

$serialized2 = serialize($heap2);
$unserialized2 = unserialize($serialized2);

echo "Unserialized array heap count: " . count($unserialized2) . "\n";

while (!$unserialized2->isEmpty()) {
    $item = $unserialized2->extract();
    echo "Array: " . json_encode($item) . "\n";
}

?>
--EXPECT--
Original heap count: 3
Unserialized heap count: 3
Extraction order: 100, 75, 50

Array heap count: 3
Unserialized array heap count: 3
Array: {"type":"array3","value":5}
Array: {"type":"array2","value":20}
Array: {"type":"array1","value":10}
