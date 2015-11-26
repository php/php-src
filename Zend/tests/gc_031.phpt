--TEST--
GC 031: gc_collect_roots() with GC turned off.
--INI--
zend.enable_gc=0
--FILE--
<?php
gc_collect_cycles();
echo "DONE\n";
?>
--EXPECTF--
DONE
