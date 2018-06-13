--TEST--
GC 037: gc_status()
--INI--
zend.enable_gc = 1
--FILE--
<?php
var_dump(gc_status());
$a = array();
$a[] =& $a;
unset($a);
gc_collect_cycles();
gc_collect_cycles();
var_dump(gc_status());
--EXPECT--
array(3) {
  ["gc_runs"]=>
  int(0)
  ["collected"]=>
  int(0)
  ["gc_threshold"]=>
  int(10001)
}
array(3) {
  ["gc_runs"]=>
  int(1)
  ["collected"]=>
  int(1)
  ["gc_threshold"]=>
  int(10001)
}
