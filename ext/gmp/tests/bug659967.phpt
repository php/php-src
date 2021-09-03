--TEST--
Bug #65997: Leak when using gc_collect_cycles with new GMP implementation
--EXTENSIONS--
gmp
--FILE--
<?php

gc_enable();
$gmp = gmp_init('10');
gc_collect_cycles();

?>
===DONE===
--EXPECT--
===DONE===
