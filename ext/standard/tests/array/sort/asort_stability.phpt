--TEST--
asort() is stable
--FILE--
<?php

$array = $origArray = array_fill(0, 1000, null);
asort($array);
var_dump($array === $origArray);

?>
--EXPECT--
bool(true)
