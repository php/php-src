--TEST--
GC 006: Simple array-object cycle
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = new stdClass();
$a->a = array();
$a->a[0] =& $a;
var_dump($a);
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECTF--
object(stdClass)#%d (1) {
  ["a"]=>
  array(1) {
    [0]=>
    *RECURSION*
  }
}
int(2)
ok
