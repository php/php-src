--TEST--
References in debug_zval_dump()
--FILE--
<?php

$r = 1;
$a = [&$r];
debug_zval_dump($a);
$a[] =& $r;
debug_zval_dump($a);
unset($a[1]);
debug_zval_dump($a);
unset($r);
// rc=1 singleton ref remains
debug_zval_dump($a);

?>
--EXPECT--
array(1) packed refcount(2){
  [0]=>
  reference refcount(2) {
    int(1)
  }
}
array(2) packed refcount(2){
  [0]=>
  reference refcount(3) {
    int(1)
  }
  [1]=>
  reference refcount(3) {
    int(1)
  }
}
array(1) packed refcount(2){
  [0]=>
  reference refcount(2) {
    int(1)
  }
}
array(1) packed refcount(2){
  [0]=>
  reference refcount(1) {
    int(1)
  }
}
