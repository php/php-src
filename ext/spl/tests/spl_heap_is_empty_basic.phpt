--TEST--
SPL: SplHeap, test trivial method to find if a heap is empty
--CREDITS--
Nathaniel McHugh nat@fishtrap.co.uk
#testfest London 2009-05-09
--FILE--
<?php

class MyHeap extends SplHeap{

public function compare($a, $b): int {
return $a < $b;
}

}


$heap = new MyHeap();
var_dump($heap->isEmpty());
$heap->insert(1);
var_dump($heap->isEmpty());
$heap->extract();
var_dump($heap->isEmpty());
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
