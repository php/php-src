--TEST--
ImmutableIterable keys() and values()
--FILE--
<?php

$it = new ImmutableIterable(['first' => new stdClass()]);
var_dump($it->keys());
var_dump($it->values());
$it = new ImmutableIterable([]);
var_dump($it->keys());
var_dump($it->values());
?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "first"
}
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
array(0) {
}
array(0) {
}