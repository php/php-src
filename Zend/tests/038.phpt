--TEST--
Trying to use lambda as array key
--FILE--
<?php

var_dump(array(function() { } => 1));

?>
--EXPECT--
array(1) {
  [Closure#1]=>
  int(1)
}
