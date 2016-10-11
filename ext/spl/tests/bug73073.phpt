--TEST--
Bug #73073: CachingIterator null dereference when convert to string
--FILE--
<?php
$it = new CachingIterator(new ArrayIterator(array()), CachingIterator::TOSTRING_USE_KEY);
var_dump((string)$it);
?>
--EXPECT--
string(0) ""
