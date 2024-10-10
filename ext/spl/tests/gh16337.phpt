--TEST--
GH-16337 (Use-after-free in SplHeap)
--FILE--
<?php

class C {
    function __toString() {
        global $heap;
        try {
            $heap->extract();
        } catch (Throwable $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            $heap->insert(1);
        } catch (Throwable $e) {
            echo $e->getMessage(), "\n";
        }
        echo $heap->top(), "\n";
        return "0";
    }
}

$heap = new SplMinHeap;
for ($i = 0; $i < 100; $i++) {
    $heap->insert((string) $i);
}
$heap->insert(new C);

?>
--EXPECT--
Heap cannot be changed when it is already being modified.
Heap cannot be changed when it is already being modified.
0
Heap cannot be changed when it is already being modified.
Heap cannot be changed when it is already being modified.
0
Heap cannot be changed when it is already being modified.
Heap cannot be changed when it is already being modified.
0
Heap cannot be changed when it is already being modified.
Heap cannot be changed when it is already being modified.
0
Heap cannot be changed when it is already being modified.
Heap cannot be changed when it is already being modified.
0
Heap cannot be changed when it is already being modified.
Heap cannot be changed when it is already being modified.
0
