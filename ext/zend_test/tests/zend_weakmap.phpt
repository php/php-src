--TEST--
Test internal weakmap API
--FILE--
<?php

$id1 = new \stdClass;
$id2 = new \stdClass;

var_dump(zend_weakmap_attach($id1, 1));
var_dump(zend_weakmap_attach($id1, 3));
var_dump(zend_weakmap_attach($id2, 2));

var_dump(zend_weakmap_dump());

var_dump(zend_weakmap_remove($id2));
var_dump(zend_weakmap_remove($id2));

var_dump(zend_weakmap_dump());

?>
--EXPECTF--
bool(true)
bool(false)
bool(true)
array(2) {
  [%d]=>
  int(1)
  [%d]=>
  int(2)
}
bool(true)
bool(false)
array(1) {
  [%d]=>
  int(1)
}