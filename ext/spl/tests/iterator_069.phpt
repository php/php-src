--TEST--
SPL: RecursiveIteratorIterator cannot be used with foreach by reference
--FILE--
<?php

$arr = array(array(1,2));
$arrOb = new ArrayObject($arr);

$recArrIt = new RecursiveArrayIterator($arrOb->getIterator());

$recItIt = new RecursiveIteratorIterator($recArrIt);

try {
    foreach ($recItIt as &$val) echo "$val\n";
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
An iterator cannot be used with foreach by reference
