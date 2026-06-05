--TEST--
SPL: RecursiveIteratorIterator - Exception thrown in nextElement which should be handled in next()
--FILE--
<?php

$recArrIt = new RecursiveArrayIterator([
    [1, 2],
    ['a', 'b'],
]);

class MyRecursiveIteratorIterator extends RecursiveIteratorIterator {

    function nextElement(): void {
        throw new Exception;
    }
}


$recItIt = new MyRecursiveIteratorIterator($recArrIt, RecursiveIteratorIterator::LEAVES_ONLY, RecursiveIteratorIterator::CATCH_GET_CHILD);

var_dump($recItIt->next());

$recItIt = new MyRecursiveIteratorIterator($recArrIt, RecursiveIteratorIterator::LEAVES_ONLY);

var_dump($recItIt->next());

?>
--EXPECTF--
NULL

Fatal error: Uncaught Exception in %s
Stack trace:
#0 [internal function]: MyRecursiveIteratorIterator->nextElement()
#1 %s: RecursiveIteratorIterator->next()
#2 {main}
  thrown in %s on line %d
