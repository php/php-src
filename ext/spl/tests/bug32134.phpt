--TEST--
Bug #32134 (Overloading offsetGet/offsetSet)
--FILE--
<?php

class myArray extends ArrayIterator
{

    public function __construct($array = array())
    {
        parent::__construct($array);
    }

    public function offsetGet($index): mixed
    {
        static $i = 0;
        echo __METHOD__ . "($index)\n";
        if (++$i > 3) exit(1);
        return parent::offsetGet($index);
    }

    public function offsetSet($index, $newval): void
    {
        echo __METHOD__ . "($index,$newval)\n";
        parent::offsetSet($index, $newval);
    }

}

$myArray = new myArray();

$myArray->offsetSet('one', 'one');
var_dump($myArray->offsetGet('one'));

$myArray['two'] = 'two';
var_dump($myArray['two']);

?>
--EXPECT--
myArray::offsetSet(one,one)
myArray::offsetGet(one)
string(3) "one"
myArray::offsetSet(two,two)
myArray::offsetGet(two)
string(3) "two"
