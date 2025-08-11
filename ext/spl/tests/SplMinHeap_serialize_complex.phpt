--TEST--
SplMinHeap serialization with nested arrays and mixed types
--FILE--
<?php
$heap = new SplMinHeap();

$heap->insert(30);
$heap->insert(10);
$heap->insert(20);

echo "Original heap count: " . count($heap) . "\n";

$serialized = serialize($heap);
$unserialized = unserialize($serialized);

echo "Unserialized heap count: " . count($unserialized) . "\n";

$extracted = [];
while (!$unserialized->isEmpty()) {
    $extracted[] = $unserialized->extract();
}
echo "Extraction order: " . implode(', ', $extracted) . "\n";

$heap2 = new SplMinHeap();
$heap2->insert(['name' => 'Alice', 'nested' => ['age' => 25, 'city' => 'NYC']]);
$heap2->insert(['name' => 'Bob', 'nested' => ['age' => 30, 'city' => 'LA']]);
$heap2->insert(['name' => 'Charlie', 'nested' => ['age' => 35, 'city' => 'SF']]);

echo "\nNested array heap count: " . count($heap2) . "\n";

$serialized2 = serialize($heap2);
$unserialized2 = unserialize($serialized2);

echo "Unserialized nested heap count: " . count($unserialized2) . "\n";

while (!$unserialized2->isEmpty()) {
    $item = $unserialized2->extract();
    echo "Person: " . $item['name'] . ", Age: " . $item['nested']['age'] . ", City: " . $item['nested']['city'] . "\n";
}

?>
--EXPECT--
Original heap count: 3
Unserialized heap count: 3
Extraction order: 10, 20, 30

Nested array heap count: 3
Unserialized nested heap count: 3
Person: Alice, Age: 25, City: NYC
Person: Bob, Age: 30, City: LA
Person: Charlie, Age: 35, City: SF
