--TEST--
SplHeap using a compare function returning by-reference
--FILE--
<?php
class Heap extends SplMinHeap {
    public function &compare($a, $b): int {
        return $a;
    }
}
$h = new Heap;
$h->insert(0);
$h->insert(0);
?>
===DONE===
--EXPECT--
===DONE===
