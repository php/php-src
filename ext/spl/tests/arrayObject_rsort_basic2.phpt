--TEST--
SPL: Test ArrayObject::sort(SORT_FLAG_REVERSE) function : basic functionality with object based store
--FILE--
<?php
/* Prototype  : int ArrayObject::sort()
 * Description: proto int ArrayIterator::sort()
 * Sort the entries by values.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::sort(SORT_FLAG_REVERSE) : basic functionality ***\n";
Class C {
	public $prop1 = 'x';
	public $prop2 = 'z';
	private $prop3 = 'a';
	public $prop4 = 'x';
}

$c = new C;
$ao1 = new ArrayObject($c);
var_dump($ao1->sort(SORT_FLAG_REVERSE));
var_dump($ao1, $c);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::sort(SORT_FLAG_REVERSE) : basic functionality ***
bool(true)
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    [0]=>
    string(1) "z"
    [1]=>
    string(1) "x"
    [2]=>
    string(1) "x"
    [3]=>
    string(1) "a"
  }
}
object(C)#1 (4) {
  [0]=>
  string(1) "z"
  [1]=>
  string(1) "x"
  [2]=>
  string(1) "x"
  [3]=>
  string(1) "a"
}
===DONE===
