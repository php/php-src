--TEST--
Test ArrayObject::uksort() function : basic functionality
--FILE--
<?php
/* Sort the entries by key using user defined function.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::uksort() : basic functionality ***\n";
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
$ao = new ArrayObject(array(3=>0, 2=>1, 5=>2, 6=>3, 1=>4));

$ao->uksort('cmp');
var_dump($ao);
?>
--EXPECT--
*** Testing ArrayObject::uksort() : basic functionality ***
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(5) {
    [6]=>
    int(3)
    [5]=>
    int(2)
    [3]=>
    int(0)
    [2]=>
    int(1)
    [1]=>
    int(4)
  }
}
