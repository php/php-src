--TEST--
SPL: ArrayIterator and foreach by reference
--FILE--
<?php

$ar = new ArrayObject(array(1));            foreach($ar as &$v) var_dump($v);
$ar = new ArrayIterator(array(2));          foreach($ar as &$v) var_dump($v);
$ar = new RecursiveArrayIterator(array(3)); foreach($ar as &$v) var_dump($v);

class ArrayIteratorEx extends ArrayIterator
{
    function current(): mixed
    {
        return ArrayIterator::current();
    }
}

$ar = new ArrayIteratorEx(array(4)); foreach($ar as $v) var_dump($v);
$ar = new ArrayIteratorEx(array(5)); foreach($ar as &$v) var_dump($v);

?>
===DONE===
--EXPECTF--
int(1)
int(2)
int(3)
int(4)

Fatal error: Uncaught Error: An iterator cannot be used with foreach by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
