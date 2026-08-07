--TEST--
SplPriorityQueue unserialization with invalid flags should throw exception
--FILE--
<?php

try {
    $data = [
        [],
        [
            'flags' => 4, // invalid flag value (4 & 3 = 0)
            'heap_elements' => []
        ]
    ];
    
    $queue = new SplPriorityQueue();
    $queue->__unserialize($data);
    echo "Should have thrown exception for invalid flags\n";
} catch (Exception $e) {
    echo 'invalid flags: ', $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $data = [
        [],
        [
            'flags' => 0,
            'heap_elements' => []
        ]
    ];
    
    $queue = new SplPriorityQueue();
    $queue->__unserialize($data);
    echo "Should have thrown exception for zero flags\n";
} catch (Exception $e) {
    echo 'zero flags: ', $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $data = [
        [],
        [
            'flags' => SplPriorityQueue::EXTR_DATA,
            'heap_elements' => []
        ]
    ];
    
    $queue = new SplPriorityQueue();
    $queue->__unserialize($data);
    echo "Valid flags accepted\n";
} catch (Exception $e) {
    echo 'Valid flags rejected: ', $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $data = [
        [],
        [
            'flags' => 999, // extra bits that should be masked to 3 (EXTR_BOTH)
            'heap_elements' => []
        ]
    ];
    
    $queue = new SplPriorityQueue();
    $queue->__unserialize($data);
    
    if ($queue->getExtractFlags() === SplPriorityQueue::EXTR_BOTH) {
        echo "Flags properly masked\n";
    } else {
        echo "Flags not properly masked, got: " . $queue->getExtractFlags() . "\n";
    }
} catch (Exception $e) {
    echo 'Flags with extra bits should be masked: ', $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
invalid flags: Exception: Invalid serialization data for SplPriorityQueue object
zero flags: Exception: Invalid serialization data for SplPriorityQueue object
Valid flags accepted
Flags properly masked
