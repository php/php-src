--TEST--
SPL: SplMaxHeap, Test method to comare elements
--CREDITS--
Chris Scott chris.scott@nstein.com
#testfest London 2009-05-09
--FILE--
<?php

class MyHeap extends SplMaxHeap
{
    public function testCompare()
    {
        return parent::compare(1);
    }
}

$heap = new MyHeap();
$heap->testCompare();

?>
--EXPECTF--
Warning: SplMaxHeap::compare() expects exactly 2 parameters, %s
