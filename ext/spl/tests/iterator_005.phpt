--TEST--
SPL: IteratorIterator and ArrayIterator/Object
--FILE--
<?php

class ArrayIteratorEx extends ArrayIterator
{
    function rewind(): void
    {
        echo __METHOD__ . "\n";
        parent::rewind();
    }
}

$it = new ArrayIteratorEx(range(0,3));

foreach(new IteratorIterator($it) as $v)
{
    var_dump($v);
}

class ArrayObjectEx extends ArrayObject
{
    function getIterator(): Iterator
    {
        echo __METHOD__ . "\n";
        return parent::getIterator();
    }
}

$it = new ArrayObjectEx(range(0,3));

foreach(new IteratorIterator($it) as $v)
{
    var_dump($v);
}

?>
--EXPECT--
ArrayIteratorEx::rewind
int(0)
int(1)
int(2)
int(3)
ArrayObjectEx::getIterator
int(0)
int(1)
int(2)
int(3)
