--TEST--
SPL: RecursiveIteratorIterator - Exception thrown in callHasChildren which should be handled in next()
--FILE--
<?php

$arr = array(1,2);
$arrOb = new ArrayObject($arr);

$recArrIt = new RecursiveArrayIterator($arrOb->getIterator());

class MyRecursiveIteratorIterator extends RecursiveIteratorIterator {

    function callHasChildren() {
        throw new Exception;
    }
}


$recItIt = new MyRecursiveIteratorIterator($recArrIt, RecursiveIteratorIterator::LEAVES_ONLY, RecursiveIteratorIterator::CATCH_GET_CHILD);

var_dump($recItIt->next());

$recItIt2 = new MyRecursiveIteratorIterator($recArrIt, RecursiveIteratorIterator::LEAVES_ONLY);

var_dump($recItIt2->next());

?>
--EXPECTF--
NULL

Fatal error: Uncaught Exception in %s
Stack trace:
#0 [internal function]: MyRecursiveIteratorIterator->callHasChildren()
#1 %s: RecursiveIteratorIterator->next()
#2 {main}
  thrown in %s on line %d
