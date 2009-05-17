--TEST--
SPL: SplHeap top, illegal number of args
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
$h = new SplMinHeap();
$h->insert(5);
// top doesn't take any args, lets see what happens if we give it one
$h->top('bogus');
?>
--EXPECTF--
Warning: SplHeap::top() expects exactly 0 parameters, 1 given in %s
