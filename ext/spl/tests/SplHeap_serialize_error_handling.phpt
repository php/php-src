--TEST--
SplHeap and SplPriorityQueue serialization error handling
--FILE--
<?php

try {
    $heap = new SplMaxHeap();
    $heap->__unserialize([]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "Wrong element count - " . $e->getMessage() . "\n";
}

try {
    $heap = new SplMinHeap();
    $heap->__unserialize(['not_array', []]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "Invalid properties type - " . $e->getMessage() . "\n";
}

try {
    $heap = new SplMaxHeap();
    $heap->__unserialize([[], 'not_array']);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "Invalid state type - " . $e->getMessage() . "\n";
}

try {
    $heap = new SplMaxHeap();
    $heap->__unserialize([[], []]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "Missing flags - " . $e->getMessage() . "\n";
}

try {
    $heap = new SplMaxHeap();
    $heap->__unserialize([[], ['flags' => 0, 'heap_elements' => 'not_array']]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "Invalid heap elements - " . $e->getMessage() . "\n";
}

try {
    $pq = new SplPriorityQueue();
    $pq->__unserialize([[], [], []]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "PQ wrong element count - " . $e->getMessage() . "\n";
}

try {
    $pq = new SplPriorityQueue();
    $pq->__unserialize([123, []]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "PQ invalid properties - " . $e->getMessage() . "\n";
}

try {
    $pq = new SplPriorityQueue();
    $pq->__unserialize([[], []]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "PQ missing flags - " . $e->getMessage() . "\n";
}

try {
    $pq = new SplPriorityQueue();
    $pq->__unserialize([[], ['flags' => 1, 'heap_elements' => ['not_array']]]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "PQ invalid element structure - " . $e->getMessage() . "\n";
}

try {
    $pq = new SplPriorityQueue();
    $pq->__unserialize([[], ['flags' => 1, 'heap_elements' => [['data' => 'test']]]]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "PQ missing priority - " . $e->getMessage() . "\n";
}

try {
    $pq = new SplPriorityQueue();
    $pq->__unserialize([[], ['flags' => 1, 'heap_elements' => [['priority' => 1]]]]);
    echo "FAIL: Should have thrown exception\n";
} catch (Exception $e) {
    echo "PQ missing data - " . $e->getMessage() . "\n";
}

?>
--EXPECT--
Wrong element count - Invalid serialization data for SplMaxHeap object
Invalid properties type - Invalid serialization data for SplMinHeap object
Invalid state type - Invalid serialization data for SplMaxHeap object
Missing flags - Invalid serialization data for SplMaxHeap object
Invalid heap elements - Invalid serialization data for SplMaxHeap object
PQ wrong element count - Invalid serialization data for SplPriorityQueue object
PQ invalid properties - Invalid serialization data for SplPriorityQueue object
PQ missing flags - Invalid serialization data for SplPriorityQueue object
PQ invalid element structure - Invalid serialization data for SplPriorityQueue object
PQ missing priority - Invalid serialization data for SplPriorityQueue object
PQ missing data - Invalid serialization data for SplPriorityQueue object
