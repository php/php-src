--TEST--
GC 031: gc_collect_roots() with GC turned off.
--INI--
zend.gc_enable=1
--FILE--
<?php
gc_collect_cycles();
echo "DONE\n";
?>
--EXPECTF--
DONE
