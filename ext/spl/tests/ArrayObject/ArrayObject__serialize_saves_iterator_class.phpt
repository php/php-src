--TEST--
ArrayObject::__serialize saves any iterator class set by ::setIteratorClass
--FILE--
<?php

class MyArrayIterator extends ArrayIterator {}
$arrayObject = new ArrayObject(array(1, 2, 3));
$arrayObject->setIteratorClass("MyArrayIterator");
$serialized = serialize($arrayObject);
$backAgain  = unserialize($serialized);
echo $backAgain->getIteratorClass(), "\n";

?>
--EXPECT--
MyArrayIterator
