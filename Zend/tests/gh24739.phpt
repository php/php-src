--TEST--
array_splice() resets type info before destroying hash table
--ENV--
a=1
--FILE--
<?php

$a = [getenv('a'), getenv('a'), getenv('a')];
$b = $a;
unset($b); // add to GC buffer

array_splice($a, 1, 1);

var_dump($a);

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
