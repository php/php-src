--TEST--
Transliterator::getErrorCode (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
echo transliterator_get_error_code(array()), "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: transliterator_get_error_code() expects parameter 1 to be Transliterator, array given in %s:%d
Stack trace:
#0 %s(%d): transliterator_get_error_code(Array)
#1 {main}
  thrown in %s on line %d
