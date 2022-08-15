--TEST--
GC 037: gc_status()
--INI--
zend.enable_gc = 1
--FILE--
<?php
$a = array();
$a[] =& $a;
unset($a);
var_dump(gc_status());
gc_collect_cycles();
gc_collect_cycles();
var_dump(gc_status());
?>
--EXPECT--
array(8) {
  ["running"]=>
  bool(false)
  ["protected"]=>
  bool(false)
  ["full"]=>
  bool(false)
  ["runs"]=>
  int(0)
  ["collected"]=>
  int(0)
  ["threshold"]=>
  int(10001)
  ["buffer_size"]=>
  int(16384)
  ["roots"]=>
  int(1)
}
array(8) {
  ["running"]=>
  bool(false)
  ["protected"]=>
  bool(false)
  ["full"]=>
  bool(false)
  ["runs"]=>
  int(1)
  ["collected"]=>
  int(1)
  ["threshold"]=>
  int(10001)
  ["buffer_size"]=>
  int(16384)
  ["roots"]=>
  int(0)
}
