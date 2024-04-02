--TEST--
GC 047: Leak after GC inside a foreach loop
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = [0, 1];
foreach($a as &$v) {
    $a[0] =& $a;
    $a[1] = array();
    $a[1][0] =& $a[1];
    $b = 1;
    $a =& $b;
    gc_collect_cycles();
    break;
}
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(2)
