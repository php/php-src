--TEST--
SPL: Test ArrayObject::arsort() function : basic functionality with array based store
--FILE--
<?php
/* Prototype  : int ArrayObject::arsort()
 * Description: proto int ArrayIterator::arsort()
 * Sort the entries by values.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::arsort() : basic functionality ***\n";

$ao1 = new ArrayObject(array(4,2,3));
$ao2 = new ArrayObject(array('a'=>4,'b'=>2,'c'=>3));
var_dump($ao1->arsort());
var_dump($ao1);
var_dump($ao2->arsort('blah'));
var_dump($ao2);
var_dump($ao2->arsort(SORT_NUMERIC));
var_dump($ao2);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::arsort() : basic functionality ***
bool(true)
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(4)
    [2]=>
    int(3)
    [1]=>
    int(2)
  }
}

Warning: arsort() expects parameter 2 to be integer, string given in %sarrayObject_arsort_basic1.php on line %d
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
    ["a"]=>
    int(4)
    ["c"]=>
    int(3)
    ["b"]=>
    int(2)
  }
}
===DONE===
