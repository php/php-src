--TEST--
GH-19543 002: GC treats ZEND_WEAKREF_TAG_MAP references as WeakMap references
--EXTENSIONS--
zend_test
--FILE--
<?php

$e = new Exception();
$a = new stdClass();
zend_weakmap_attach($e, $a);
unset($a);
$e2 = $e;
unset($e2); // add to roots
gc_collect_cycles();

?>
==DONE==
--EXPECT--
==DONE==
