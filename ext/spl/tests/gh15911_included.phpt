--TEST--
GH-15911 (Appending AppendIterator containing AppendIterator to the contained AppendIterator)
--FILE--
<?php

$iterator = new AppendIterator();
$other = new AppendIterator();
$other->append( $iterator );
$iterator->append( $other );

?>
--EXPECTF--
Fatal error: Uncaught ValueError: AppendIterator::append(): Argument #1 ($iterator) must not contain the iterator being appended to in %s:%d
Stack trace:
#0 %s(%d): AppendIterator->append(Object(AppendIterator))
#1 {main}
  thrown in %s on line %d
