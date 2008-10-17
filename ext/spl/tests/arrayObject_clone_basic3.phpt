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
  [u"p"]=>
  unicode(9) "C::p.orig"
  [u"dynamic1"]=>
  unicode(45) "new prop added to $wrappedObject before clone"
  [u"dynamic2"]=>
  unicode(44) "new prop added to $wrappedObject after clone"
  [u"new.iAO"]=>
  unicode(35) "new element added $innerArrayObject"
  [u"new.oAO"]=>
  unicode(38) "new element added to $outerArrayObject"
}
object(ArrayObject)#2 (1) {
  [u"storage":u"ArrayObject":private]=>
  object(C)#1 (5) {
    [u"p"]=>
    unicode(9) "C::p.orig"
    [u"dynamic1"]=>
    unicode(45) "new prop added to $wrappedObject before clone"
    [u"dynamic2"]=>
    unicode(44) "new prop added to $wrappedObject after clone"
    [u"new.iAO"]=>
    unicode(35) "new element added $innerArrayObject"
    [u"new.oAO"]=>
    unicode(38) "new element added to $outerArrayObject"
  }
}
object(ArrayObject)#3 (1) {
  [u"storage":u"ArrayObject":private]=>
  object(ArrayObject)#2 (1) {
    [u"storage":u"ArrayObject":private]=>
    object(C)#1 (5) {
      [u"p"]=>
      unicode(9) "C::p.orig"
      [u"dynamic1"]=>
      unicode(45) "new prop added to $wrappedObject before clone"
      [u"dynamic2"]=>
      unicode(44) "new prop added to $wrappedObject after clone"
      [u"new.iAO"]=>
      unicode(35) "new element added $innerArrayObject"
      [u"new.oAO"]=>
      unicode(38) "new element added to $outerArrayObject"
    }
  }
}
object(ArrayObject)#4 (1) {
  [u"storage":u"ArrayObject":private]=>
  array(3) {
    [u"p"]=>
    unicode(9) "C::p.orig"
    [u"dynamic1"]=>
    unicode(45) "new prop added to $wrappedObject before clone"
    [u"new.coAO"]=>
    unicode(44) "new element added to $clonedOuterArrayObject"
  }
}
