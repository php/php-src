--TEST--
SPL: Test ArrayObject::asort() function : basic functionality with object based store
--FILE--
<?php
/* Prototype  : int ArrayObject::asort()
 * Description: proto int ArrayIterator::asort()
 * Sort the entries by values. 
 * Source code: ext/spl/spl_array.c
 * Alias to functions: 
 */

echo "*** Testing ArrayObject::asort() : basic functionality ***\n";
Class C {
	public $prop1 = 'x';
	public $prop2 = 'z';
	private $prop3 = 'a';
	public $prop4 = 'x';
}

$c = new C;
$ao1 = new ArrayObject($c);
var_dump($ao1->asort());
var_dump($ao1, $c);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::asort() : basic functionality ***
bool(true)
object(ArrayObject)#2 (1) {
  [u"storage":u"ArrayObject":private]=>
  object(C)#1 (4) {
    [u"prop3":u"C":private]=>
    unicode(1) "a"
    [u"prop1"]=>
    unicode(1) "x"
    [u"prop4"]=>
    unicode(1) "x"
    [u"prop2"]=>
    unicode(1) "z"
  }
}
object(C)#1 (4) {
  [u"prop3":u"C":private]=>
  unicode(1) "a"
  [u"prop1"]=>
  unicode(1) "x"
  [u"prop4"]=>
  unicode(1) "x"
  [u"prop2"]=>
  unicode(1) "z"
}
===DONE===
