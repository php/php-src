--TEST--
Bug #62609: Allow implementing Traversable on abstract classes (fail)
--FILE--
<?php

abstract class AbstractTraversable implements Traversable {}

class NonAbstractTraversable extends AbstractTraversable {}

?>
--EXPECT--
Fatal error: Class NonAbstractTraversable must implement interface Traversable as part of either Iterator or IteratorAggregate in Unknown on line 0
