--TEST--
Bug #72369 (array_merge() produces references in PHP7)
--FILE--
<?php
$x = 'xxx';
$d = ['test' => &$x];
unset($x);
$a = ['test' => 'yyy'];
$a = array_merge($a, $d);
debug_zval_dump($a);
?>
--EXPECTF--
array(1) refcount(%d){
  ["test"]=>
  string(3) "xxx" interned
}
