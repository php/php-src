--TEST--
SPL: Test ArrayObject::krsort() function : basic functionality with object base store
--FILE--
<?php
/* Prototype  : int ArrayObject::krsort()
 * Description: proto int ArrayIterator::krsort()
 * Sort the entries by key.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::krsort() : basic functionality ***\n";
Class C {
	public $x = 'prop1';
	public $z = 'prop2';
	public $a = 'prop3';
	private $b = 'prop4';
}

$c = new C;
$ao1 = new ArrayObject($c);
var_dump($ao1->krsort());
var_dump($ao1, $c);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::krsort() : basic functionality ***
bool(true)
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    ["z"]=>
    string(5) "prop2"
    ["x"]=>
    string(5) "prop1"
    ["a"]=>
    string(5) "prop3"
    ["b":"C":private]=>
    string(5) "prop4"
  }
}
object(C)#1 (4) {
  ["z"]=>
  string(5) "prop2"
  ["x"]=>
  string(5) "prop1"
  ["a"]=>
  string(5) "prop3"
  ["b":"C":private]=>
  string(5) "prop4"
}
===DONE===
