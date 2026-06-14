--TEST--
SPL: Test ArrayObject::ksort() function : basic functionality with object base store
--FILE--
<?php
/* Sort the entries by key.
 * Source code: ext/spl/spl_array.c
 * Alias to functions:
 */

echo "*** Testing ArrayObject::ksort() : basic functionality ***\n";
Class C {
    public $x = 'prop1';
    public $z = 'prop2';
    public $a = 'prop3';
    private $b = 'prop4';
}

$c = new C;
$ao1 = new ArrayObject($c);
var_dump($ao1->ksort());
var_dump($ao1, $c);
?>
--EXPECTF--
*** Testing ArrayObject::ksort() : basic functionality ***

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
bool(true)
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#1 (4) {
    ["b":"C":private]=>
    string(5) "prop4"
    ["a"]=>
    string(5) "prop3"
    ["x"]=>
    string(5) "prop1"
    ["z"]=>
    string(5) "prop2"
  }
}
object(C)#1 (4) {
  ["b":"C":private]=>
  string(5) "prop4"
  ["a"]=>
  string(5) "prop3"
  ["x"]=>
  string(5) "prop1"
  ["z"]=>
  string(5) "prop2"
}
