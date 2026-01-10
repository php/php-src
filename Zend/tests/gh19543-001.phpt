--TEST--
GH-19543 001: GC treats ZEND_WEAKREF_TAG_MAP references as WeakMap references
--EXTENSIONS--
zend_test
--FILE--
<?php

$e = new Exception();
$a = new stdClass();
zend_weakmap_attach($e, $a);
unset($a);
gc_collect_cycles();

?>
==DONE==
--EXPECT--
==DONE==
