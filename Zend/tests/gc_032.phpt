--TEST--
GC 032: Crash in GC because of invalid reference counting
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = array();
$b =& $a;
$a[0] = $a;
var_dump($a);
$a = array(array());
$b =& $a;
$a[0][0] = $a;
var_dump($a);
?>
--EXPECT--
array(1) {
  [0]=>
  array(0) {
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(0) {
      }
    }
  }
}
