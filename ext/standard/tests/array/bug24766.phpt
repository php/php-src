--TEST--
Bug #24766 (strange result array from unpack)
--FILE--
<?php

error_reporting(E_ALL);

$a=unpack('C2', "\0224V");
debug_zval_dump($a);
$k=array_keys($a);
debug_zval_dump($k);

$i=$k[0];

echo $a[$i],"\n";

?>
--EXPECT--
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
18
