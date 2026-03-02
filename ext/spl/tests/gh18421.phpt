--TEST--
GH-18421 (Integer overflow with large numbers in LimitIterator)
--FILE--
<?php

$a = array('zero' => 0, 'one' => 1, 'two' => 2, 'three' => 3, 'four' => 4, 'five' => 5);
try {
    foreach (new LimitIterator(new ArrayIterator($a), PHP_INT_MAX, PHP_INT_MAX) as $k => $v)
    {
    }
} catch (OutOfBoundsException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Seek position %d is out of range
