--TEST--
SPL: Cloning nested ArrayObjects. 
--FILE--
<?php
class C {
	public $p = 'C::p.orig';
}

$wrappedObject = new C;
$innerArrayObject = new ArrayObject($wrappedObject);

$outerArrayObject =  new ArrayObject($innerArrayObject);

$wrappedObject->dynamic1 = 'new prop added to $wrappedObject before clone';
$clonedOuterArrayObject = clone $outerArrayObject;
$wrappedObject->dynamic2 = 'new prop added to $wrappedObject after clone';

$innerArrayObject['new.iAO'] = 'new element added $innerArrayObject';
$outerArrayObject['new.oAO'] = 'new element added to $outerArrayObject';
$clonedOuterArrayObject['new.coAO'] = 'new element added to $clonedOuterArrayObject';

var_dump($wrappedObject, $innerArrayObject, $outerArrayObject, $clonedOuterArrayObject);
?>
--EXPECTF--
object(C)#1 (5) {
  ["p"]=>
  string(9) "C::p.orig"
  ["dynamic1"]=>
  string(45) "new prop added to $wrappedObject before clone"
  ["dynamic2"]=>
  string(44) "new prop added to $wrappedObject after clone"
  ["new.iAO"]=>
  string(35) "new element added $innerArrayObject"
  ["new.oAO"]=>
  string(38) "new element added to $outerArrayObject"
}
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#1 (5) {
    ["p"]=>
    string(9) "C::p.orig"
    ["dynamic1"]=>
    string(45) "new prop added to $wrappedObject before clone"
    ["dynamic2"]=>
    string(44) "new prop added to $wrappedObject after clone"
    ["new.iAO"]=>
    string(35) "new element added $innerArrayObject"
    ["new.oAO"]=>
    string(38) "new element added to $outerArrayObject"
  }
}
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  object(ArrayObject)#2 (1) {
    ["storage":"ArrayObject":private]=>
    object(C)#1 (5) {
      ["p"]=>
      string(9) "C::p.orig"
      ["dynamic1"]=>
      string(45) "new prop added to $wrappedObject before clone"
      ["dynamic2"]=>
      string(44) "new prop added to $wrappedObject after clone"
      ["new.iAO"]=>
      string(35) "new element added $innerArrayObject"
      ["new.oAO"]=>
      string(38) "new element added to $outerArrayObject"
    }
  }
}
object(ArrayObject)#4 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    ["p"]=>
    string(9) "C::p.orig"
    ["dynamic1"]=>
    string(45) "new prop added to $wrappedObject before clone"
    ["new.coAO"]=>
    string(44) "new element added to $clonedOuterArrayObject"
  }
}
