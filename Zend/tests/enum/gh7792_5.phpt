--TEST--
GH-7792 (Refer to enum as enum instead of class)
--FILE--
<?php

enum Foo implements Traversable {}

?>
--EXPECT--
Fatal error: Enum Foo must implement interface Traversable as part of either Iterator or IteratorAggregate in Unknown on line 0
