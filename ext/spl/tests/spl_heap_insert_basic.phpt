--TEST--
SPL: SplHeap, Test method to insert into heap
--CREDITS--
Chris Scott chris.scott@nstein.com
#testfest London 2009-05-09
--FILE--
<?php
class MyHeap extends SplHeap
{
    public function compare($a, $b)
    {
        return $a < $b;
    }
}

$heap = new MyHeap();
$heap->insert(1,2);
?>
--EXPECTF--
Warning: SplHeap::insert() expects exactly 1 parameter, %s
