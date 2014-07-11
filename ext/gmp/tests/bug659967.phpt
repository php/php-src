--TEST--
Bug #65997: Leak when using gc_collect_cycles with new GMP implementation
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

gc_enable();
$gmp = gmp_init('10');
gc_collect_cycles();

?>
===DONE===
--EXPECT--
===DONE===
