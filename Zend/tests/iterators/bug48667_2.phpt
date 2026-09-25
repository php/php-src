--TEST--
Bug #48667 (Implementing both iterator and iteratoraggregate)
--FILE--
<?php

abstract class A implements IteratorAggregate, Iterator { }

?>
--EXPECTF--
Fatal error: Class A cannot implement both Iterator and IteratorAggregate at the same time in %s on line %d
