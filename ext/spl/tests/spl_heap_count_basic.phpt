--TEST--
SPL: SplHeap, Test spl_heap_object_count_elements (spl_heap.c:490) for returning count() failure for Heaps
--CREDITS--
Chris Scott chris.scott@nstein.com
#testfest London 2009-05-09
--FILE--
<?php

class MyHeap extends SplHeap
{
    public function compare($a,$b)
    {
        return ($a < $b);
    }

    public function count() // override count to force failure
    {
        throw new Exception('Cause count to fail');
        return parent::count();
    }
}


$heap = new MyHeap();
$heap->insert(1);
count($heap);// refers to MyHeap->count() method

?>
--EXPECTF--
Fatal error: Uncaught exception 'Exception' with message 'Cause count to fail' in %s
Stack trace:
#0 [internal function]: MyHeap->count()
#1 %s count(Object(MyHeap))
#2 {main}
  thrown in %s on line %d
