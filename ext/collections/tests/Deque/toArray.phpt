--TEST--
Collections\Deque toArray()
--FILE--
<?php

$it = new Collections\Deque(['first' => new stdClass()]);
var_dump($it->toArray());
var_dump($it->toArray());
$it = new Collections\Deque([]);
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