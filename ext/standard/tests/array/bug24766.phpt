--TEST--
Bug #24766 (strange result array from unpack)
--FILE--
<?php

error_reporting(E_ALL);

$a = unpack('C2', "\0224V");
$b = array(1 => 18, 2 => 52);
var_dump($a, $b);
$k = array_keys($a);
$l = array_keys($b);
var_dump($k, $l);
$i=$k[0];
var_dump($a[$i]);
$i=$l[0];
var_dump($b[$i]);
?>
--EXPECT--
array(2) {
  [1]=>
  int(18)
  [2]=>
  int(52)
}
array(2) {
  [1]=>
  int(18)
  [2]=>
  int(52)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
int(18)
int(18)
