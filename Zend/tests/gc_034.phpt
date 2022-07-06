--TEST--
GC 034: GC in request shutdown and resource list destroy
--FILE--
<?php
/* run with valgrind */
$a = array(fopen(__FILE__, 'r'));
$a[] = &$a;
?>
==DONE==
--EXPECT--
==DONE==
