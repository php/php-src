--TEST--
ArrayObject::__unserialize() with wrong-case iterator class name fails with wrong case
--FILE--
<?php
class MyIterator extends ArrayIterator {}

$ao = new ArrayObject([1, 2, 3]);
$ao->setIteratorClass(MyIterator::class);

$serialized = serialize($ao);
$ao2 = unserialize($serialized);
echo $ao2->getIteratorClass() . "\n";

// Manually craft serialized data with wrong-case iterator class
$wrong = 'O:11:"ArrayObject":4:{i:0;i:0;i:1;a:3:{i:0;i:1;i:1;i:2;i:2;i:3;}i:2;a:0:{}i:3;s:10:"MYITERATOR";}';
$ao3 = unserialize($wrong);
echo $ao3->getIteratorClass() . "\n";
?>
--EXPECTF--
MyIterator

Fatal error: Uncaught UnexpectedValueException: Cannot deserialize ArrayObject with iterator class 'MYITERATOR'; no such class exists in %s:%d
Stack trace:
#0 [internal function]: ArrayObject->__unserialize(Array)
#1 %s(13): unserialize('O:11:"ArrayObje...')
#2 {main}
  thrown in %s on line %d
