--TEST--
SPL: InfiniteIterator
--FILE--
<?php

class ArrayIteratorEx extends ArrayIterator
{
    function rewind()
    {
        echo __METHOD__ . "\n";
        parent::rewind();
    }
    function valid()
    {
        echo __METHOD__ . "\n";
        return parent::valid();
    }
    function current()
    {
        echo __METHOD__ . "\n";
        return parent::current();
    }
    function key()
    {
        echo __METHOD__ . "\n";
        return parent::key();
    }
    function next()
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
