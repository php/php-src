--TEST--
SPL: RecursiveIteratorIterator - Test where the case is RS_SELF and mode is CHILD_FIRST
--FILE--
<?php

$recArrIt = new RecursiveArrayIterator([
    [1, 2],
    ['a', 'b'],
]);

class MyRecursiveIteratorIterator extends RecursiveIteratorIterator {

    function nextelement(): void {
        echo __METHOD__."\n";
    }
}


$recItIt = new MyRecursiveIteratorIterator($recArrIt, RecursiveIteratorIterator::CHILD_FIRST);

foreach ($recItIt as $key => $val) echo "$key\n";

?>
--EXPECT--
MyRecursiveIteratorIterator::nextelement
0
MyRecursiveIteratorIterator::nextelement
1
MyRecursiveIteratorIterator::nextelement
0
MyRecursiveIteratorIterator::nextelement
0
MyRecursiveIteratorIterator::nextelement
1
MyRecursiveIteratorIterator::nextelement
1
