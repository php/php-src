--TEST--
GC 015: Object as root of cycle
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = new stdClass();
$c =& $a;
$b = $a;
$a->a = $a;
$a->b = "xxx";
unset($c);
unset($a);
unset($b);
var_dump(gc_collect_cycles() > 0);
echo "ok\n";
?>
--EXPECT--
bool(true)
ok
