--TEST--
SPL: CachingIterator and __toString and flags = 0
--FILE--
<?php

function test($it)
{
	foreach($it as $v)
	{
		var_dump((string)$it);
	}
}

$ar = new ArrayIterator(array(1, 2, 3));

test(new CachingIterator($ar, 0));

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught BadMethodCallException: CachingIterator does not fetch string value (see CachingIterator::__construct) in %s:%d
Stack trace:
#0 %s(%d): CachingIterator->__toString()
#1 %s(%d): test(Object(CachingIterator))
#2 {main}
  thrown in %s on line %d
