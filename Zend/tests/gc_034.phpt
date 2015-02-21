--TEST--
GC 033: GC in request shutdown and resource list destroy
--FILE--
<?php
/* run with valgrind */
$a = array(fopen(__FILE__, 'r'));
$a[] = &$a;
?>
==DONE==
--EXPECT--
==DONE==
