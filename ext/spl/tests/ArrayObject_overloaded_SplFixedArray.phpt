--TEST--
SplFixedArray properties is incompatible with ArrayObject
--FILE--
<?php
$ao = new ArrayObject([1, 2, 3]);
$fixedArray = new SplFixedArray(1);
$fixedArray[0] = new SplDoublyLinkedList();
try {
  // See GH-15918: this *should* fail to not break invariants
  $ao->exchangeArray($fixedArray);
} catch (InvalidArgumentException $e) {
  echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Overloaded object of type SplFixedArray is not compatible with ArrayObject
