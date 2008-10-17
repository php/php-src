--TEST--
SPL: Test ArrayObject::natcasesort() function : basic functionality 
--FILE--
<?php
/* Prototype  : int ArrayObject::natcasesort()
 * Description: proto int ArrayIterator::natcasesort()
 Sort the entries by values using case insensitive "natural order" algorithm. 
 * Source code: ext/spl/spl_array.c
 * Alias to functions: 
 */

echo "*** Testing ArrayObject::natcasesort() : basic functionality ***\n";

$ao1 = new ArrayObject(array('boo10','boo1','boo2','boo22','BOO5'));
$ao2 = new ArrayObject(array('a'=>'boo10','b'=>'boo1','c'=>'boo2','d'=>'boo22','e'=>'BOO5'));
var_dump($ao1->natcasesort());
var_dump($ao1);
var_dump($ao2->natcasesort('blah'));
var_dump($ao2);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::natcasesort() : basic functionality ***
bool(true)
object(ArrayObject)#1 (1) {
  [u"storage":u"ArrayObject":private]=>
  array(5) {
    [1]=>
    unicode(4) "boo1"
    [2]=>
    unicode(4) "boo2"
    [4]=>
    unicode(4) "BOO5"
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
    [u"b"]=>
    unicode(4) "boo1"
    [u"c"]=>
    unicode(4) "boo2"
    [u"e"]=>
    unicode(4) "BOO5"
    [u"a"]=>
    unicode(5) "boo10"
    [u"d"]=>
    unicode(5) "boo22"
  }
}
===DONE===
