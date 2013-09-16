--TEST--
count(new EmptyIterator) should return zero
--FILE--
<?php
$it = new EmptyIterator;
var_dump(count($it));
--EXPECT--
int(0)
