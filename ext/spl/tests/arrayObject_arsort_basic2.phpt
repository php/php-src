--TEST--
SPL: Test ArrayObject::arsort() function : basic functionality with object based store
--FILE--
<?php
/* Prototype  : int ArrayObject::arsort()
 * Description: proto int ArrayIterator::arsort()
 * Sort the entries by values.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::arsort() : basic functionality ***\n";
Class C {
	public $prop1 = 'x';
	public $prop2 = 'z';
	private $prop3 = 'a';
	public $prop4 = 'x';
}

$c = new C;
$ao1 = new ArrayObject($c);
var_dump($ao1->arsort());
var_dump($ao1, $c);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::arsort() : basic functionality ***
bool(true)
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    ["prop2"]=>
    string(1) "z"
    ["prop1"]=>
    string(1) "x"
    ["prop4"]=>
    string(1) "x"
    ["prop3":"C":private]=>
    string(1) "a"
  }
}
object(C)#1 (4) {
  ["prop2"]=>
  string(1) "z"
  ["prop1"]=>
  string(1) "x"
  ["prop4"]=>
  string(1) "x"
  ["prop3":"C":private]=>
  string(1) "a"
}
===DONE===
