--TEST--
Bug #69227 (Use after free in zval_scan caused by spl_object_storage_get_gc)
--INI--
zend.enable_gc=1
--FILE--
<?php

$s = new SplObjectStorage();
$s->attach($s);
gc_collect_cycles();
echo "ok";
?>
--EXPECT--
ok
