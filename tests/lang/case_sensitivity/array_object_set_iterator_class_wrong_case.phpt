--TEST--
ArrayObject::setIteratorClass() with wrong-case class name fails with wrong case
--FILE--
<?php
class MyArrayIterator extends ArrayIterator {}

$ao = new ArrayObject([1, 2, 3]);
$ao->setIteratorClass("MYARRAYITERATOR");
echo $ao->getIteratorClass() . "\n";

$ao->setIteratorClass("myarrayiterator");
echo $ao->getIteratorClass() . "\n";

$ao->setIteratorClass("MyArrayIterator");
echo $ao->getIteratorClass() . "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: ArrayObject::setIteratorClass(): Argument #1 ($iteratorClass) must be a class name derived from ArrayIterator, MYARRAYITERATOR given in %s:%d
Stack trace:
#0 %s(5): ArrayObject->setIteratorClass('MYARRAYITERATOR')
#1 {main}
  thrown in %s on line %d
