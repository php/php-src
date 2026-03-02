--TEST--
SPL: InfiniteIterator
--FILE--
<?php

class ArrayIteratorEx extends ArrayIterator
{
    function rewind(): void
    {
        echo __METHOD__ . "\n";
        parent::rewind();
    }
    function valid(): bool
    {
        echo __METHOD__ . "\n";
        return parent::valid();
    }
    function current(): mixed
    {
        echo __METHOD__ . "\n";
        return parent::current();
    }
    function key(): string|int|null
    {
        echo __METHOD__ . "\n";
        return parent::key();
    }
    function next(): void
    {
        echo __METHOD__ . "\n";
        parent::next();
    }
}

$it = new InfiniteIterator(new ArrayIteratorEx(range(0,2)));

$pos =0;

foreach ($it as $v) {
    var_dump($v);
    if ($pos++ > 5) {
        break;
    }
}

?>
--EXPECT--
ArrayIteratorEx::rewind
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(0)
ArrayIteratorEx::next
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(1)
ArrayIteratorEx::next
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(2)
ArrayIteratorEx::next
ArrayIteratorEx::valid
ArrayIteratorEx::rewind
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(0)
ArrayIteratorEx::next
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(1)
ArrayIteratorEx::next
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(2)
ArrayIteratorEx::next
ArrayIteratorEx::valid
ArrayIteratorEx::rewind
ArrayIteratorEx::valid
ArrayIteratorEx::current
ArrayIteratorEx::key
int(0)
