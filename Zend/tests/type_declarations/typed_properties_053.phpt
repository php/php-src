--TEST--
Default values of callable properties
--FILE--
<?php
class A {
	public callable $a1;
	public callable $a2 = "foo";
	public callable $a3 = array("A","bar");
}
$obj = new A;
var_dump($obj);
?>
--EXPECT--
object(A)#1 (2) {
  ["a1"]=>
  uninitialized(callable)
  ["a2"]=>
  string(3) "foo"
  ["a3"]=>
  array(2) {
    [0]=>
    string(1) "A"
    [1]=>
    string(3) "bar"
  }
}
