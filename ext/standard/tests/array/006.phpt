--TEST--
Test array_pop behaviour
--POST--
--GET--
--FILE--
<?php

array_pop($GLOBALS);

$a = array("foo", "bar", "fubar");
$b = array("3" => "foo", "4" => "bar", "5" => "fubar");
$c = array("a" => "foo", "b" => "bar", "c" => "fubar");

/* simple array */
echo array_pop($a), "\n";
var_dump($a);

/* numerical assoc indices */
echo array_pop($b), "\n";
var_dump($b);

/* assoc indices */
echo array_pop($c), "\n";
var_dump($c);

?>
--EXPECT--
fubar
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
fubar
array(2) {
  [3]=>
  string(3) "foo"
  [4]=>
  string(3) "bar"
}
fubar
array(2) {
  ["a"]=>
  string(3) "foo"
  ["b"]=>
  string(3) "bar"
}
