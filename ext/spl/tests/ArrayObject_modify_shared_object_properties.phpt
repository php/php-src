--TEST--
Modifications to ArrayObjects should not affect shared properties tables
--FILE--
<?php

$obj = (object)['a' => 1, 'b' => 2];
$ao = new ArrayObject($obj);
$arr = (array) $obj;
$ao['a'] = 42;
var_dump($arr);

?>
--EXPECT--
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
