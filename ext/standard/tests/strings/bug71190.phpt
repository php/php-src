--TEST--
Bug #71190 (substr_replace converts integers in original $search array to strings)
--FILE--
<?php
$b = [0, 1, 2];

var_dump($b);
substr_replace("test", $b, "1");
var_dump($b);
?>
--EXPECT--
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
