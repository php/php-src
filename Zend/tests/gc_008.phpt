--TEST--
GC 008: Unreferenced object cycle
--INI--
zend.enable_gc=1
--FILE--
<?php
$a = new stdClass();
$a->a = new stdClass();
$a->a->a = $a->a;
var_dump($a->a);
var_dump(gc_collect_cycles());
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECT--
object(DynamicObject)#2 (1) {
  ["a"]=>
  *RECURSION*
}
int(0)
int(1)
ok
