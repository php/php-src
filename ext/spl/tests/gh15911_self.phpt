--TEST--
GH-15911 (Infinite recursion when appending AppendIterator to itself)
--FILE--
<?php

$iterator = new AppendIterator();

$iterator->append($iterator);
?>
--EXPECTF--
Fatal error: Uncaught ValueError: AppendIterator::append(): Argument #1 ($iterator) must be different than the iterator being appended to in %s:%d
Stack trace:
#0 %s(%d): AppendIterator->append(Object(AppendIterator))
#1 {main}
  thrown in %s on line %d
