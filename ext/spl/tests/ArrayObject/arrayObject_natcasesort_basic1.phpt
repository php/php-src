--TEST--
SPL: Test ArrayObject::natcasesort() function : basic functionality
--FILE--
<?php
/* Sort the entries by values using case insensitive "natural order" algorithm.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::natcasesort() : basic functionality ***\n";

$ao1 = new ArrayObject(array('boo10','boo1','boo2','boo22','BOO5'));
$ao2 = new ArrayObject(array('a'=>'boo10','b'=>'boo1','c'=>'boo2','d'=>'boo22','e'=>'BOO5'));
var_dump($ao1->natcasesort());
var_dump($ao1);
try {
    var_dump($ao2->natcasesort('blah'));
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($ao2);
?>
--EXPECT--
*** Testing ArrayObject::natcasesort() : basic functionality ***
bool(true)
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(5) {
    [1]=>
    string(4) "boo1"
    [2]=>
    string(4) "boo2"
    [4]=>
    string(4) "BOO5"
    [0]=>
    string(5) "boo10"
    [3]=>
    string(5) "boo22"
  }
}
ArrayObject::natcasesort() expects exactly 0 arguments, 1 given
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(5) {
    ["a"]=>
    string(5) "boo10"
    ["b"]=>
    string(4) "boo1"
    ["c"]=>
    string(4) "boo2"
    ["d"]=>
    string(5) "boo22"
    ["e"]=>
    string(4) "BOO5"
  }
}
