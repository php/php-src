--TEST--
GC 024: GC and objects with non-standard handlers
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
$a = new ArrayObject();
$a[0] = $a;
//var_dump($a);
unset($a);
var_dump(gc_collect_cycles());
echo "ok\n";
?>
--EXPECT--
int(1)
ok
