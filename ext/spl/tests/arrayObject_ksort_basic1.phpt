--TEST--
SPL: Test ArrayObject::ksort() function : basic functionality with array based store 
--FILE--
<?php
/* Prototype  : int ArrayObject::ksort()
 * Description: proto int ArrayIterator::ksort()
 * Sort the entries by key. 
 * Source code: ext/spl/spl_array.c
 * Alias to functions: 
 */

echo "*** Testing ArrayObject::ksort() : basic functionality ***\n";
$ao1 = new ArrayObject(array(4,2,3));
$ao2 = new ArrayObject(array('b'=>4,'a'=>2,'q'=>3, 99=>'x'));
var_dump($ao1->ksort());
var_dump($ao1);
var_dump($ao2->ksort('blah'));
var_dump($ao2);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::ksort() : basic functionality ***
bool(true)
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(4)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
bool(true)
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(4) {
    ["a"]=>
    int(2)
    ["b"]=>
    int(4)
    ["q"]=>
    int(3)
    [99]=>
    string(1) "x"
  }
}
===DONE===
