--TEST--
SPL: Test ArrayObject::ksort() function : basic functionality with object base store 
--FILE--
<?php
/* Prototype  : int ArrayObject::ksort()
 * Description: proto int ArrayIterator::ksort()
 * Sort the entries by key. 
 * Source code: ext/spl/spl_array.c
 * Alias to functions: 
 */

echo "*** Testing ArrayObject::ksort() : basic functionality ***\n";
Class C {
	public $x = 'prop1';
	public $z = 'prop2';
	public $a = 'prop3';
	private $b = 'prop4';
}

$c = new C;
$ao1 = new ArrayObject($c);
var_dump($ao1->ksort());
var_dump($ao1, $c);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::ksort() : basic functionality ***
bool(true)
object(ArrayObject)#2 (1) {
  [u"storage":u"ArrayObject":private]=>
  object(C)#1 (4) {
    [u"b":u"C":private]=>
    unicode(5) "prop4"
    [u"a"]=>
    unicode(5) "prop3"
    [u"x"]=>
    unicode(5) "prop1"
    [u"z"]=>
    unicode(5) "prop2"
  }
}
object(C)#1 (4) {
  [u"b":u"C":private]=>
  unicode(5) "prop4"
  [u"a"]=>
  unicode(5) "prop3"
  [u"x"]=>
  unicode(5) "prop1"
  [u"z"]=>
  unicode(5) "prop2"
}
===DONE===
