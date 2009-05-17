--TEST--
SPL: SplCachingIterator, Test method to set flags for caching iterator
--CREDITS--
Chris Scott chris.scott@nstein.com
#testfest London 2009-05-09
--FILE--
<?php

$ai = new ArrayIterator(array('foo', 'bar'));

$ci = new CachingIterator($ai);
$ci->setFlags(); //expects arg

?>
--EXPECTF--
Warning: CachingIterator::setFlags() expects exactly 1 parameter, %s
