--TEST--
GC 020: GC detach reference with assign
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = array();
$a[0] =& $a;
$a[1] = array();
$a[1][0] =& $a[1];
$a = 1;
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
int(1)
ok
