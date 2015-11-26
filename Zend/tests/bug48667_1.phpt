--TEST--
Bug #48667 (Implementing both iterator and iteratoraggregate)
--FILE--
<?php

abstract class A implements Iterator, IteratorAggregate { }

?>
--EXPECTF--
Fatal error: Class A cannot implement both IteratorAggregate and Iterator at the same time in %s on line %d
