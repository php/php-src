--TEST--
Test that the "clone with" respects #[AllowDynamicProperties]
--FILE--
<?php

$obj1 = new stdClass();
$obj2 = clone $obj1 with ["foo" => 1, "bar" => ""];
$obj3 = clone $obj1 with ["foo" => 2, "bar" => []];

var_dump($obj1);
var_dump($obj2);
var_dump($obj3);

?>
--EXPECTF--
object(stdClass)#1 (%d) {
}
object(stdClass)#2 (%d) {
  ["foo"]=>
  int(1)
  ["bar"]=>
  string(0) ""
}
object(stdClass)#3 (%d) {
  ["foo"]=>
  int(2)
  ["bar"]=>
  array(0) {
  }
}
