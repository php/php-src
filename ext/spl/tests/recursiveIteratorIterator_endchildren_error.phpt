--TEST--
SPL: RecursiveIteratorIterator - Exception thrown in endchildren which should be handled in next()
--FILE--
<?php

$arr = array(array(1,2));
$arrOb = new ArrayObject($arr);

$recArrIt = new RecursiveArrayIterator($arrOb->getIterator());

class MyRecursiveIteratorIterator extends RecursiveIteratorIterator {

    function endchildren() {
        throw new Exception;
    }
}


$recItIt = new MyRecursiveIteratorIterator($recArrIt, RecursiveIteratorIterator::LEAVES_ONLY, RecursiveIteratorIterator::CATCH_GET_CHILD);

foreach ($recItIt as $val) echo "$val\n";

$recItIt2 = new MyRecursiveIteratorIterator($recArrIt, RecursiveIteratorIterator::LEAVES_ONLY);

echo "===NEXT LOOP===\n";

foreach ($recItIt2 as $val) echo "$val\n";

?>
--EXPECTF--
1
2
===NEXT LOOP===
1
2

Fatal error: Uncaught Exception in %s
Stack trace:
#0 [internal function]: MyRecursiveIteratorIterator->endchildren()
#1 %s: RecursiveIteratorIterator->next()
#2 {main}
  thrown in %s on line %d
