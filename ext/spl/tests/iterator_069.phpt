--TEST--
SPL: RecursiveIteratorIterator cannot be used with foreach by reference
--FILE--
<?php

$recArrIt = new RecursiveArrayIterator([
    [1, 2],
    ['a', 'b'],
]);

$recItIt = new RecursiveIteratorIterator($recArrIt);

foreach ($recItIt as &$val) echo "$val\n";

?>
--EXPECTF--
Fatal error: Uncaught Error: An iterator cannot be used with foreach by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
