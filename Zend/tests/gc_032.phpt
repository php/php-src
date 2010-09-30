--TEST--
GC 032: Crash in GC because of invalid reference counting
--FILE--
<?php
$a = array();
$b =& $a;
$a[0] = $a;
debug_zval_dump($a);
$a = array(array());
$b =& $a;
$a[0][0] = $a;
debug_zval_dump($a);
?>
--EXPECT--
array(1) refcount(1){
  [0]=>
  array(1) refcount(3){
    [0]=>
    array(1) refcount(3){
      [0]=>
      *RECURSION*
    }
  }
}
array(1) refcount(1){
  [0]=>
  array(1) refcount(3){
    [0]=>
    array(1) refcount(1){
      [0]=>
      array(1) refcount(3){
        [0]=>
        array(1) refcount(1){
          [0]=>
          *RECURSION*
        }
      }
    }
  }
}
