--TEST--
Bug #24766 (strange result array from unpack)
--FILE--
<?php

error_reporting(E_ALL);

$a = unpack('C2', "\0224V");
$b = array(1 => 18, 2 => 52);
debug_zval_dump($a, $b);
$k = array_keys($a);
$l = array_keys($b);
debug_zval_dump($k, $l);
$i=$k[0];
var_dump($a[$i]);
$i=$l[0];
var_dump($b[$i]);
?>
--EXPECT--
array(2) refcount(2){
  [1]=>
  long(18) refcount(1)
  [2]=>
  long(52) refcount(1)
}
array(2) refcount(2){
  [1]=>
  long(18) refcount(1)
  [2]=>
  long(52) refcount(1)
}
array(2) refcount(2){
  [0]=>
  long(1) refcount(1)
  [1]=>
  long(2) refcount(1)
}
array(2) refcount(2){
  [0]=>
  long(1) refcount(1)
  [1]=>
  long(2) refcount(1)
}
int(18)
int(18)
