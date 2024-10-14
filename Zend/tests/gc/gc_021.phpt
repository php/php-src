--TEST--
GC 021: GC detach reference with assign by reference
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = array();
$a[0] =& $a;
$a[1] = array();
$a[1][0] =& $a[1];
$b = 1;
$a =& $b;
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
int(2)
ok
