--TEST--
GH-17935: RCn array modification violation with ArrayObject serialize
--FILE--
<?php

$o = new ArrayObject();
$o['a'] = 'a';
$s = $o->__serialize();
$o['b'] = 'b';
var_dump($o, $s);

?>
--EXPECT--
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    ["a"]=>
    string(1) "a"
    ["b"]=>
    string(1) "b"
  }
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  array(1) {
    ["a"]=>
    string(1) "a"
  }
  [2]=>
  array(0) {
  }
  [3]=>
  NULL
}
