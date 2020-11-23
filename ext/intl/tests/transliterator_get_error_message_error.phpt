--TEST--
Transliterator::getErrorMessage (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
echo transliterator_get_error_message(array()), "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: transliterator_get_error_message(): Argument #1 ($transliterator) must be of type Transliterator, array given in %s:%d
Stack trace:
#0 %s(%d): transliterator_get_error_message(Array)
#1 {main}
  thrown in %s on line %d
