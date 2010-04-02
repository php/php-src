--TEST--
GC 007: Unreferensed array cycle
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = array(array());
$a[0][0] =& $a[0];
var_dump($a[0]);
var_dump(gc_collect_cycles());
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    *RECURSION*
  }
}
int(0)
int(1)
ok
