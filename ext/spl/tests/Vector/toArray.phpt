--TEST--
Vector toArray()
--FILE--
<?php

$it = new Vector(['first' => new stdClass()], false);
var_dump($it->toArray());
var_dump($it->toArray());
$it = new Vector([]);
var_dump($it->toArray());
var_dump($it->toArray());
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
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