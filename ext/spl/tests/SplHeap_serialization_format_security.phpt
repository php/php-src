--TEST--
SplHeap serialization format - indexed array format prevents conflicts
--FILE--
<?php

$heap = new SplMaxHeap();
$heap->insert(100);
$heap->insert(50);
$heap->insert(200);

$serialized_data = $heap->__serialize();

echo "Serialization data structure:\n";
echo "Is indexed array: " . (isset($serialized_data[0], $serialized_data[1]) && count($serialized_data) === 2 ? 'YES' : 'NO') . "\n";
echo "Index 0 (properties) is array: " . (is_array($serialized_data[0]) ? 'YES' : 'NO') . "\n";
echo "Index 1 (internal state) is array: " . (is_array($serialized_data[1]) ? 'YES' : 'NO') . "\n";

$internal_state = $serialized_data[1];
echo "Internal state contains 'flags': " . (array_key_exists('flags', $internal_state) ? 'YES' : 'NO') . "\n";
echo "Internal state contains 'heap_elements': " . (array_key_exists('heap_elements', $internal_state) ? 'YES' : 'NO') . "\n";
echo "Flags value is numeric: " . (is_numeric($internal_state['flags']) ? 'YES' : 'NO') . "\n";
echo "Heap elements is array: " . (is_array($internal_state['heap_elements']) ? 'YES' : 'NO') . "\n";
echo "Heap elements count: " . count($internal_state['heap_elements']) . "\n";

$min_heap = new SplMinHeap();
$min_heap->insert(100);
$min_heap->insert(50);

$min_data = $min_heap->__serialize();

$pq = new SplPriorityQueue();
$pq->insert('data', 10);
$pq->setExtractFlags(SplPriorityQueue::EXTR_DATA);

$pq_data = $pq->__serialize();

$pq_internal = $pq_data[1];
if (isset($pq_internal['heap_elements']) && is_array($pq_internal['heap_elements'])) {
    $first_element = $pq_internal['heap_elements'][0];
    echo "PQ elements have 'data' key: " . (array_key_exists('data', $first_element) ? 'YES' : 'NO') . "\n";
    echo "PQ elements have 'priority' key: " . (array_key_exists('priority', $first_element) ? 'YES' : 'NO') . "\n";
}

?>
--EXPECT--
Serialization data structure:
Is indexed array: YES
Index 0 (properties) is array: YES
Index 1 (internal state) is array: YES
Internal state contains 'flags': YES
Internal state contains 'heap_elements': YES
Flags value is numeric: YES
Heap elements is array: YES
Heap elements count: 3
PQ elements have 'data' key: YES
PQ elements have 'priority' key: YES
