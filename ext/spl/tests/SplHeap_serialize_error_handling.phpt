--TEST--
SplHeap and SplPriorityQueue unserialization error handling
--FILE--
<?php

// Test malformed data cases
$invalid_cases = [
    // Wrong array count
    [],
    [[], [], []],
    
    // Invalid properties type
    ['not_array', []],
    [123, []],
    
    // Invalid state type  
    [[], 'not_array'],
    [[], 123],
    
    // Missing flags
    [[], []],
    
    // Invalid flags type
    [[], ['flags' => 'not_int']],
    
    // Missing heap_elements
    [[], ['flags' => 0]],
    
    // Invalid heap_elements type
    [[], ['flags' => 0, 'heap_elements' => 'not_array']],
    [[], ['flags' => 0, 'heap_elements' => 123]],
];

foreach ($invalid_cases as $i => $case) {
    try {
        $heap = new SplMaxHeap();
        $heap->__unserialize($case);
        echo "Case $i: UNEXPECTED SUCCESS\n";
    } catch (Exception $e) {
        echo "Case $i: " . $e->getMessage() . "\n";
    }
}

$pq_invalid_cases = [
    // Invalid flags for PQ
    [[], ['flags' => 0]],
    
    // Invalid element structure
    [[], ['flags' => 1, 'heap_elements' => ['not_array']]],
    
    // Missing data/priority keys
    [[], ['flags' => 1, 'heap_elements' => [['data' => 'test']]]],
    [[], ['flags' => 1, 'heap_elements' => [['priority' => 1]]]],
    [[], ['flags' => 1, 'heap_elements' => [[]]]],
];

foreach ($pq_invalid_cases as $i => $case) {
    try {
        $pq = new SplPriorityQueue();
        $pq->__unserialize($case);
        echo "PQ Case $i: UNEXPECTED SUCCESS\n";
    } catch (Exception $e) {
        echo "PQ Case $i: " . $e->getMessage() . "\n";
    }
}

?>
--EXPECT--
Case 0: Invalid serialization data for SplMaxHeap object
Case 1: Invalid serialization data for SplMaxHeap object
Case 2: Invalid serialization data for SplMaxHeap object
Case 3: Invalid serialization data for SplMaxHeap object
Case 4: Invalid serialization data for SplMaxHeap object
Case 5: Invalid serialization data for SplMaxHeap object
Case 6: Invalid serialization data for SplMaxHeap object
Case 7: Invalid serialization data for SplMaxHeap object
Case 8: Invalid serialization data for SplMaxHeap object
Case 9: Invalid serialization data for SplMaxHeap object
Case 10: Invalid serialization data for SplMaxHeap object
PQ Case 0: Invalid serialization data for SplPriorityQueue object
PQ Case 1: Invalid serialization data for SplPriorityQueue object
PQ Case 2: Invalid serialization data for SplPriorityQueue object
PQ Case 3: Invalid serialization data for SplPriorityQueue object
PQ Case 4: Invalid serialization data for SplPriorityQueue object
