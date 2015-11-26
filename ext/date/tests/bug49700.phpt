--TEST--
Bug #49700 (memory leaks in php_date.c if garbage collector is enabled)
--INI--
date.timezone=GMT
--FILE--
<?php
gc_enable();
$objs = array();
$objs[1] = new DateTime();
gc_collect_cycles();
unset($objs);
echo "OK\n";
?>
--EXPECT--
OK
