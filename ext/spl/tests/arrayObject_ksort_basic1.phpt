--TEST--
SPL: Test ArrayObject::ksort() function : basic functionality with array based store
--FILE--
<?php
/* Sort the entries by key.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::ksort() : basic functionality ***\n";
$ao1 = new ArrayObject(array(4,2,3));
$ao2 = new ArrayObject(array('b'=>4,'a'=>2,'q'=>3, 99=>'x'));
var_dump($ao1->ksort());
var_dump($ao1);
try {
    var_dump($ao2->ksort('blah'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($ao2);
var_dump($ao2->ksort(SORT_STRING));
var_dump($ao2);
?>
--EXPECTF--
*** Testing ArrayObject::ksort() : basic functionality ***
bool(true)
object(ArrayObject)#%d (1) {
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
ArrayObject::ksort(): Argument #1 ($flags) must be of type int, string given
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
    [99]=>
    string(1) "x"
    ["a"]=>
    int(2)
    ["b"]=>
    int(4)
    ["q"]=>
    int(3)
  }
}
