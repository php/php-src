--TEST--
SPL: Priority queue count, illegal number of args
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
$h = new SplPriorityQueue();
$h->count(1);
?>
--EXPECTF--
Warning: SplPriorityQueue::count() expects exactly 0 parameters, 1 given in %s
