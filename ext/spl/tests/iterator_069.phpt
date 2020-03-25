--TEST--
SPL: RecursiveIteratorIterator cannot be used with foreach by reference
--FILE--
<?php

$arr = array(array(1,2));
$arrOb = new ArrayObject($arr);

$recArrIt = new RecursiveArrayIterator($arrOb->getIterator());

$recItIt = new RecursiveIteratorIterator($recArrIt);

foreach ($recItIt as &$val) echo "$val\n";

?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: An iterator cannot be used with foreach by reference in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
