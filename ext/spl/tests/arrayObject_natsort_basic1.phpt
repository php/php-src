--TEST--
SPL: Test ArrayObject::natsort() function : basic functionality 
--FILE--
<?php
/* Prototype  : int ArrayObject::natsort()
 * Description: proto int ArrayIterator::natsort()
 Sort the entries by values using "natural order" algorithm. 
 * Source code: ext/spl/spl_array.c
 * Alias to functions: 
 */

echo "*** Testing ArrayObject::natsort() : basic functionality ***\n";

$ao1 = new ArrayObject(array('boo10','boo1','boo2','boo22','BOO5'));
$ao2 = new ArrayObject(array('a'=>'boo10','b'=>'boo1','c'=>'boo2','d'=>'boo22','e'=>'BOO5'));
var_dump($ao1->natsort());
var_dump($ao1);
var_dump($ao2->natsort('blah'));
var_dump($ao2);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::natsort() : basic functionality ***
bool(true)
object(ArrayObject)#1 (1) {
  [u"storage":u"ArrayObject":private]=>
  array(5) {
    [4]=>
    unicode(4) "BOO5"
    [1]=>
    unicode(4) "boo1"
    [2]=>
    unicode(4) "boo2"
    [0]=>
    unicode(5) "boo10"
    [3]=>
    unicode(5) "boo22"
  }
}
bool(true)
object(ArrayObject)#2 (1) {
  [u"storage":u"ArrayObject":private]=>
  array(5) {
    [u"e"]=>
    unicode(4) "BOO5"
    [u"b"]=>
    unicode(4) "boo1"
    [u"c"]=>
    unicode(4) "boo2"
    [u"a"]=>
    unicode(5) "boo10"
    [u"d"]=>
    unicode(5) "boo22"
  }
}
===DONE===
