--TEST--
SPL: SplHeap with overriden compare()
--FILE--
<?php
class SplMinHeap2 extends SplMinHeap {
    public function compare($a, $b) {
        return -parent::compare($a,$b);
    }
}
$h = new SplMinHeap2();
$h->insert(1);
$h->insert(6);
$h->insert(5);
$h->insert(2);
var_dump($h->top());

class SplMaxHeap2 extends SplMaxHeap {
    public function compare($a, $b) {
        return -parent::compare($a,$b);
    }
}
$h = new SplMaxHeap2();
$h->insert(1);
$h->insert(6);
$h->insert(5);
$h->insert(2);
var_dump($h->top());
?>
--EXPECT--
int(6)
int(1)
