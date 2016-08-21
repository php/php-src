--TEST--
Bug #72907 (null pointer deref, segfault in gc_remove_zval_from_buffer (zend_gc.c:260))
--FILE--
<?php

$a = 0;

($a->a = &$E) + ($b = $a->b->i -= 0);

?>
--EXPECTF--
Warning: Attempt to modify property of non-object in %sbug72907.php on line %d

Warning: Attempt to modify property of non-object in %sbug72907.php on line %d

Warning: Creating default object from empty value in %sbug72907.php on line %d

Notice: Undefined property: stdClass::$i in %sbug72907.php on line %d
