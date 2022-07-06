--TEST--
Bug #75155: AppendIterator::append() is broken when appending another AppendIterator
--FILE--
<?php

$array_a = new ArrayIterator(array('a', 'b', 'c'));
$array_b = new ArrayIterator(array('d', 'e', 'f'));

$iterator = new AppendIterator;
$iterator->append($array_a);

$iterator2 = new AppendIterator;
$iterator2->append($iterator);
$iterator2->append($array_b);

foreach ($iterator2 as $current) {
    echo $current;
}

?>
--EXPECT--
abcdef
