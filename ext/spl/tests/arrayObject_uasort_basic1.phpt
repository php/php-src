--TEST--
SPL: Test ArrayObject::uasort() function : basic functionality 
--FILE--
<?php
/* Prototype  : int ArrayObject::uasort(callback cmp_function)
 * Description: proto int ArrayIterator::uasort(callback cmp_function)
 Sort the entries by values user defined function. 
 * Source code: ext/spl/spl_array.c
 * Alias to functions: 
 */

echo "*** Testing ArrayObject::uasort() : basic functionality ***\n";

// Reverse sorter
function cmp($value1, $value2) {
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 < $value2) {
    return 1;
  }
  else
    return -1;
}
$ao = new ArrayObject(array(2,3,1));

$ao->uasort('cmp');
var_dump($ao);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::uasort() : basic functionality ***
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(3) {
    [1]=>
    int(3)
    [0]=>
    int(2)
    [2]=>
    int(1)
  }
}
===DONE===
