--TEST--
SPL: Cloning an instance of ArrayObject which wraps an object. 
--FILE--
<?php
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
--EXPECTF--
object(C)#1 (3) {
  [u"p1"]=>
  unicode(32) "new prop added to c before clone"
  [u"p2"]=>
  unicode(31) "new prop added to c after clone"
  [u"new.ao1"]=>
  unicode(24) "new element added to ao1"
}
object(ArrayObject)#2 (1) {
  [u"storage":u"ArrayObject":private]=>
  object(C)#1 (3) {
    [u"p1"]=>
    unicode(32) "new prop added to c before clone"
    [u"p2"]=>
    unicode(31) "new prop added to c after clone"
    [u"new.ao1"]=>
    unicode(24) "new element added to ao1"
  }
}
object(ArrayObject)#3 (1) {
  [u"storage":u"ArrayObject":private]=>
  array(2) {
    [u"p1"]=>
    unicode(32) "new prop added to c before clone"
    [u"new.ao2"]=>
    unicode(24) "new element added to ao2"
  }
}
