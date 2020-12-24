--TEST--
Objects with overloaded get_properties are incompatible with ArrayObject
--XFAIL--
SplFixedArray has migrated to get_properties_for; find new test case
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
try {
    $ao->exchangeArray(new SplFixedArray);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump($ao);

?>
--EXPECT--
Overloaded object of type SplFixedArray is not compatible with ArrayObject
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
