--TEST--
SPL: ArrayIterator overloading
--FILE--
<?php

class ArrayIteratorEx extends ArrayIterator
{
    function rewind(): void
    {
        echo __METHOD__ . "\n";
        ArrayIterator::rewind();
    }

    function valid(): bool
    {
        echo __METHOD__ . "\n";
        return ArrayIterator::valid();
    }

    function key(): string|int|null
    {
        echo __METHOD__ . "\n";
        return ArrayIterator::key();
    }

    function current(): mixed
    {
        echo __METHOD__ . "\n";
        return ArrayIterator::current();
    }

    function next(): void
    {
        echo __METHOD__ . "\n";
        ArrayIterator::next();
    }
}

$ar = new ArrayIteratorEx(array(1,2));
foreach($ar as $k => $v)
{
    var_dump($k);
    var_dump($v);
}

?>
--EXPECT--
ArrayIteratorEx::rewind
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(0)
int(1)
ArrayIteratorEx::next
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(1)
int(2)
ArrayIteratorEx::next
ArrayIteratorEx::valid
