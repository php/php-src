--TEST--
SplHeap should not accept unserialize data when it is corrupted or under modification
--FILE--
<?php

class MyHeap extends SplMaxHeap {
    public function compare($a, $b): int {
        global $array;
        static $counter = 0;
        if ($counter++ === 0)
            $this->__unserialize($array);
        return $a < $b ? -1 : ($a == $b ? 0 : 1);
    }
}

$heap = new SplMaxHeap;
$heap->insert(1);
$array = $heap->__serialize();

$heap = new MyHeap;
$heap->insert(0);
try {
    $heap->insert(2);
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Heap cannot be changed when it is already being modified.
