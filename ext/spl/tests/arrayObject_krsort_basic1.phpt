--TEST--
SPL: Test ArrayObject::krsort() function : basic functionality with array based store
--FILE--
<?php
/* Prototype  : int ArrayObject::krsort()
 * Description: proto int ArrayIterator::krsort()
 * Sort the entries by key.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::krsort() : basic functionality ***\n";
$ao1 = new ArrayObject(array(4,2,3));
$ao2 = new ArrayObject(array('b'=>4,'a'=>2,'q'=>3, 99=>'x'));
var_dump($ao1->krsort());
var_dump($ao1);
var_dump($ao2->krsort('blah'));
var_dump($ao2);
var_dump($ao2->krsort(SORT_STRING));
var_dump($ao2);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::krsort() : basic functionality ***
bool(true)
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [2]=>
    int(3)
    [1]=>
    int(2)
    [0]=>
    int(4)
  }
}

Warning: krsort() expects parameter 2 to be integer, string given in %sarrayObject_krsort_basic1.php on line %d
bool(false)
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(4) {
    ["b"]=>
    int(4)
    ["a"]=>
    int(2)
    ["q"]=>
    int(3)
    [99]=>
    string(1) "x"
  }
}
bool(true)
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(4) {
    ["q"]=>
    int(3)
    ["b"]=>
    int(4)
    ["a"]=>
    int(2)
    [99]=>
    string(1) "x"
  }
}
===DONE===
