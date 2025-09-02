--TEST--
SPL: RecursiveIteratorIterator - Exception thrown in endchildren which should be handled in next()
--FILE--
<?php

$recArrIt = new RecursiveArrayIterator([
    [1, 2],
    ['a', 'b'],
]);

class MyRecursiveIteratorIterator extends RecursiveIteratorIterator {

    function endchildren(): void {
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
a
b
===NEXT LOOP===
1
2

Fatal error: Uncaught Exception in %s
Stack trace:
#0 %s(%d): MyRecursiveIteratorIterator->endchildren()
#1 {main}
  thrown in %s on line %d
