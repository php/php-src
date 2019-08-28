--TEST--
GC 009: Unreferenced array-object cycle
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = array();
$a[0] = new StdClass();
$a[0]->a = array();
$a[0]->a[0] =& $a[0];
var_dump($a[0]);
var_dump(gc_collect_cycles());
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECTF--
object(StdClass)#%d (1) {
  ["a"]=>
  array(1) {
    [0]=>
    *RECURSION*
  }
}
int(0)
int(2)
ok
