--TEST--
Bug #78436: Broken GC for SplPriorityQueue
--FILE--
<?php 

$pqueue = new SplPriorityQueue();
$pqueue->insert($pqueue, 1);

$pqueue2 = new SplPriorityQueue();
$pqueue2->insert(1, $pqueue);

?>
===DONE===
--EXPECT--
===DONE===
