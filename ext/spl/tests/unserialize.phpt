--TEST--
SPL: unserialize with no data (for PHPUnit)
--FILE--
<?php

$types = array('SplDoublyLinkedList', 'SplObjectStorage', 'ArrayObject');

foreach ($types as $type) {
	// serialize an empty new object
	$exp = serialize(new $type());
	// hack to instanciate an object without constructor
	$str = sprintf('C:%d:"%s":0:{}', strlen($type), $type);
	$obj = unserialize($str);
	var_dump($obj);
	// serialize result
	$out = serialize($obj);
	// both should match
	var_dump($exp === $out);
}
?>
===DONE===
--EXPECTF--
object(SplDoublyLinkedList)#%d (2) {
  ["flags":"SplDoublyLinkedList":private]=>
  int(0)
  ["dllist":"SplDoublyLinkedList":private]=>
  array(0) {
  }
}
bool(true)
object(SplObjectStorage)#%d (1) {
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
bool(true)
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
bool(true)
===DONE===
