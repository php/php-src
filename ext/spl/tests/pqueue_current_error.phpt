--TEST--
SPL: SplPriorityQueue current on empty queue should give null
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
$h = new SplPriorityQueue();
var_dump($h->current());
?>
--EXPECT--
NULL
