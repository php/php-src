--TEST--
Objects with overloaded get_properties are incompatible with ArrayObject
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
try {
    $ao->exchangeArray(new DateInterval('P1D'));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump($ao);

?>
--EXPECT--
Overloaded object of type DateInterval is not compatible with ArrayObject
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
