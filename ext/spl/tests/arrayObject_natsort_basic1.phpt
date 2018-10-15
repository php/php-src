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
--EXPECT--
*** Testing ArrayObject::natsort() : basic functionality ***
bool(true)
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(5) {
    [4]=>
    string(4) "BOO5"
    [1]=>
    string(4) "boo1"
    [2]=>
    string(4) "boo2"
    [0]=>
    string(5) "boo10"
    [3]=>
    string(5) "boo22"
  }
}
bool(true)
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(5) {
    ["e"]=>
    string(4) "BOO5"
    ["b"]=>
    string(4) "boo1"
    ["c"]=>
    string(4) "boo2"
    ["a"]=>
    string(5) "boo10"
    ["d"]=>
    string(5) "boo22"
  }
}
===DONE===
