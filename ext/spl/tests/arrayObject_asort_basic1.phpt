--TEST--
SPL: Test ArrayObject::asort() function : basic functionality with array based store
--FILE--
<?php
/* Prototype  : int ArrayObject::asort()
 * Description: proto int ArrayIterator::asort()
 * Sort the entries by values.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::asort() : basic functionality ***\n";

$ao1 = new ArrayObject(array(4,2,3));
$ao2 = new ArrayObject(array('a'=>4,'b'=>2,'c'=>3));
var_dump($ao1->asort());
var_dump($ao1);
var_dump($ao2->asort('blah'));
var_dump($ao2);
var_dump($ao2->asort(SORT_NUMERIC));
var_dump($ao2);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::asort() : basic functionality ***
bool(true)
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [1]=>
    int(2)
    [2]=>
    int(3)
    [0]=>
    int(4)
  }
}

Warning: asort() expects parameter 2 to be int, string given in %sarrayObject_asort_basic1.php on line %d
bool(false)
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    ["a"]=>
    int(4)
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
  }
}
bool(true)
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
    ["a"]=>
    int(4)
  }
}
===DONE===
