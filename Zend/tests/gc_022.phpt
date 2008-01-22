--TEST--
GC 022: GC detach reference in executor's PZVAL_UNLOCK()
--INI--
error_reporting=0
--FILE--
<?php
$a = array(array());
$a[0][0] =& $a[0];
$s = array(1) + unserialize(serialize(&$a[0]));
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
int(1)
ok
