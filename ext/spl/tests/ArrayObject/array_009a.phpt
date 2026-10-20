--TEST--
SPL: ArrayIterator implementing RecursiveIterator
--FILE--
<?php

class MyRecursiveArrayIterator extends ArrayIterator implements RecursiveIterator
{
    function hasChildren(): bool
    {
        return is_array($this->current());
    }

    function getChildren(): MyRecursiveArrayIterator
    {
        return new MyRecursiveArrayIterator($this->current());
    }
}

$array = array(1, 2 => array(21, 22 => array(221, 222), 23 => array(231)), 3);

$dir = new RecursiveIteratorIterator(new MyRecursiveArrayIterator($array), RecursiveIteratorIterator::LEAVES_ONLY);

foreach ($dir as $file) {
    print "$file\n";
}

?>
--EXPECT--
1
21
221
222
231
3
