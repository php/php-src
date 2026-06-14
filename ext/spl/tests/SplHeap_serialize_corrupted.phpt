--TEST--
SplHeap and SplPriorityQueue serialization fails when corrupted
--FILE--
<?php

class ThrowingHeap extends SplMaxHeap {
    public function compare($a, $b): int {
        if ($a === 'throw' || $b === 'throw') {
            throw new Exception('Comparison failed');
        }
        return parent::compare($a, $b);
    }
}

$heap = new ThrowingHeap();
$heap->insert(1);
$heap->insert(2);

try {
    $heap->insert('throw');
} catch (Exception $e) {
    // no-op, heap should now be corrupted
}

echo "Heap is corrupted: " . ($heap->isCorrupted() ? 'YES' : 'NO') . "\n";

try {
    serialize($heap);
    echo "FAIL: Serialization should have thrown\n";
} catch (Exception $e) {
    echo "Serialization failed: " . $e->getMessage() . "\n";
}

class ThrowingPQ extends SplPriorityQueue {
    public function compare($priority1, $priority2): int {
        if ($priority1 === 'throw' || $priority2 === 'throw') {
            throw new Exception('Priority comparison failed');
        }
        return parent::compare($priority1, $priority2);
    }
}

$pq = new ThrowingPQ();
$pq->insert('data1', 1);
$pq->insert('data2', 2);

try {
    $pq->insert('data3', 'throw');
} catch (Exception $e) {
    // no-op, queue is corrupted
}

echo "PriorityQueue is corrupted: " . ($pq->isCorrupted() ? 'YES' : 'NO') . "\n";

try {
    serialize($pq);
    echo "FAIL: PQ Serialization should have thrown\n";
} catch (Exception $e) {
    echo "PQ Serialization failed: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
Heap is corrupted: YES
Serialization failed: Heap is corrupted, heap properties are no longer ensured.
PriorityQueue is corrupted: YES
PQ Serialization failed: Heap is corrupted, heap properties are no longer ensured.
