--TEST--
SPL: Test ArrayObject::asort() function : basic functionality with object based store
--FILE--
<?php
/* Prototype  : int ArrayObject::asort()
 * Description: proto int ArrayIterator::asort()
 * Sort the entries by values. 
 * Source code: ext/spl/spl_array.c
 * Alias to functions: 
 */

echo "*** Testing ArrayObject::asort() : basic functionality ***\n";
Class C {
	public $prop1 = 'x';
	public $prop2 = 'z';
	private $prop3 = 'a';
	public $prop4 = 'x';
}

$c = new C;
$ao1 = new ArrayObject($c);
var_dump($ao1->asort());
var_dump($ao1, $c);
?>
===DONE===
--EXPECTF--
*** Testing ArrayObject::asort() : basic functionality ***
bool(true)
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    ["prop3":"C":private]=>
    string(1) "a"
    ["prop1"]=>
    string(1) "x"
    ["prop4"]=>
    string(1) "x"
    ["prop2"]=>
    string(1) "z"
  }
}
object(C)#1 (4) {
  ["prop3":"C":private]=>
  string(1) "a"
  ["prop1"]=>
  string(1) "x"
  ["prop4"]=>
  string(1) "x"
  ["prop2"]=>
  string(1) "z"
}
===DONE===
