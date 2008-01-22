--TEST--
GC  015: Object as root of cycle
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
var_dump(gc_collect_cycles());
echo "ok\n";
?>
--EXPECT--
int(2)
ok
