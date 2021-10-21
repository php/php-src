--TEST--
Test internal weakmap API
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--FILE--
<?php

$id1 = new \stdClass;
$id2 = new \stdClass;

var_dump(zend_weakmap_attach($id1, 1));
var_dump(zend_weakmap_attach($id1, 3));
var_dump(zend_weakmap_attach($id2, 2));

var_dump(zend_weakmap_dump());

unset($id1);

var_dump(zend_weakmap_dump());

var_dump(zend_weakmap_remove($id2));
var_dump(zend_weakmap_remove($id2));

var_dump(zend_weakmap_dump());

var_dump(zend_weakmap_attach($id2, $id2));

var_dump(zend_weakmap_dump());

?>
--EXPECTF--
bool(true)
bool(false)
bool(true)
array(2) {
  [%s]=>
  int(1)
  [%s]=>
  int(2)
}
array(1) {
  [%s]=>
  int(2)
}
bool(true)
bool(false)
array(0) {
}
bool(true)
array(1) {
  [%s]=>
  object(stdClass)#2 (0) {
  }
}