--TEST--
SPL: SplCachingIterator, Test method to convert current element to string
--CREDITS--
Chris Scott chris.scott@nstein.com
#testfest London 2009-05-09
--FILE--
<?php

$ai = new ArrayIterator(array(new stdClass(), new stdClass()));
$ci = new CachingIterator($ai);
var_dump(
$ci->__toString() // if conversion to string is done by echo, for example, an exception is thrown. Invoking __toString explicitly covers different code.
);
?>
--EXPECT--
string(0) ""
