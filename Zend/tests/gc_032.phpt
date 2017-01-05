--TEST--
GC 032: Crash in GC because of invalid reference counting
--INI--
zend.enable_gc=1
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
--EXPECTF--
array(1) refcount(%d){
  [0]=>
  array(1) refcount(%d){
    [0]=>
    array(1) refcount(%d){
      [0]=>
      *RECURSION*
    }
  }
}
array(1) refcount(%d){
  [0]=>
  array(1) refcount(%d){
    [0]=>
    array(1) refcount(%d){
      [0]=>
      array(1) refcount(%d){
        [0]=>
        array(1) refcount(%d){
          [0]=>
          *RECURSION*
        }
      }
    }
  }
}
