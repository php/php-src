--TEST--
Test array_shift behaviour
--POST--
--GET--
--FILE--
<?php

array_shift($GLOBALS);

$a = array("foo", "bar", "fubar");
$b = array("3" => "foo", "4" => "bar", "5" => "fubar");
$c = array("a" => "foo", "b" => "bar", "c" => "fubar");

/* simple array */
echo array_shift($a), "\n";
var_dump($a);

/* numerical assoc indices */
echo array_shift($b), "\n";
var_dump($b);

/* assoc indices */
echo array_shift($c), "\n";
var_dump($c);

?>
--EXPECT--
foo
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  string(5) "fubar"
}
foo
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  string(5) "fubar"
}
foo
array(2) {
  ["b"]=>
  string(3) "bar"
  ["c"]=>
  string(5) "fubar"
}
