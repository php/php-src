--TEST--
GC 036: Memleaks in self-referenced array
--INI--
zend.enable_gc = 1
--FILE--
<?php
function &foo() {
    $a = [];
    $a[] =& $a;
    return $a;
}
function bar() {
    gc_collect_cycles();
}
bar(foo());
echo "ok\n";
?>
--EXPECT--
ok
