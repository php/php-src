--TEST--
SPL: Cloning an instance of ArrayObject which wraps an object.
--FILE--
<?php

#[AllowDynamicProperties]
class C { }

$c = new C;
$ao1 =  new ArrayObject($c);
$c->p1 = 'new prop added to c before clone';

$ao2 = clone $ao1;

$c->p2 = 'new prop added to c after clone';
$ao1['new.ao1'] = 'new element added to ao1';
$ao2['new.ao2'] = 'new element added to ao2';
var_dump($c, $ao1, $ao2);
?>
--EXPECT--
object(C)#1 (3) {
  ["p1"]=>
  string(32) "new prop added to c before clone"
  ["p2"]=>
  string(31) "new prop added to c after clone"
  ["new.ao1"]=>
  string(24) "new element added to ao1"
}
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#1 (3) {
    ["p1"]=>
    string(32) "new prop added to c before clone"
    ["p2"]=>
    string(31) "new prop added to c after clone"
    ["new.ao1"]=>
    string(24) "new element added to ao1"
  }
}
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    ["p1"]=>
    string(32) "new prop added to c before clone"
    ["new.ao2"]=>
    string(24) "new element added to ao2"
  }
}
