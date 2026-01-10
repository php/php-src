--TEST--
GH-16574 (Incorrect error "undefined method" messages)
--CREDITS--
YuanchengJiang
--FILE--
<?php
$i = new ArrayIterator([1,1,1,1,1]);
$i = new CachingIterator($i, CachingIterator::FULL_CACHE);
$i->doesnotexist("x");
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined method CachingIterator::doesnotexist() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
