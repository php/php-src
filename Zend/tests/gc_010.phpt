--TEST--
GC 010: Cycle with reference to $GLOBALS
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = array();
$a[] =& $a;
var_dump($a);
$a[] =& $GLOBALS;
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
array(1) {
  [0]=>
  *RECURSION*
}
int(1)
ok
