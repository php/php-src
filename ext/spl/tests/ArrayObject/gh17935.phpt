--TEST--
GH-17935 (Assertion failure Zend/zend_hash.c)
--FILE--
<?php
$o = new ArrayObject();
$data = $o->__serialize();
$o['a'] = 'b';
var_dump($data[1], $o->getArrayCopy());

$o = new ArrayIterator(['a' => 1]);
$data = $o->__serialize();
unset($o['a']);
var_dump($data[1], $o->getArrayCopy());
?>
--EXPECT--
array(0) {
}
array(1) {
  ["a"]=>
  string(1) "b"
}
array(1) {
  ["a"]=>
  int(1)
}
array(0) {
}
