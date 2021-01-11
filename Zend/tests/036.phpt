--TEST--
Trying to use lambda in array offset
--FILE--
<?php

$test[function(){}] = 1;
var_dump($test);

?>
--EXPECT--
array(1) {
  [Closure#1]=>
  int(1)
}
