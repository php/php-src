--TEST--
SPL: ArrayIterator and foreach by reference
--FILE--
<?php

$ar = new ArrayObject(array(1));            foreach($ar as &$v) var_dump($v);
$ar = new ArrayIterator(array(2));          foreach($ar as &$v) var_dump($v);
$ar = new RecursiveArrayIterator(array(3)); foreach($ar as &$v) var_dump($v);

class ArrayIteratorEx extends ArrayIterator
{
    function current()
    {
        return ArrayIterator::current();
    }
}

$ar = new ArrayIteratorEx(array(4)); foreach($ar as $v) var_dump($v);
try {
    $ar = new ArrayIteratorEx(array(5)); foreach($ar as &$v) var_dump($v);
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
An iterator cannot be used with foreach by reference
